# EE6470 midterm project

## Algorithm - Merge sort

![](https://i.imgur.com/dnBMlDE.png)

這裡我所實作的演算法是 merge sort，因為我在操作的時候都是在 array 上，所以只要做 `combine` 的部分即可， `divide` 可以跳過。

## version 1 (one stage)

### BASIC

以下是負責 I/O 的部分。
```cpp
	// input
	sc_dt::sc_biguint<128> array;
	int MSB, LSB;
#ifndef NATIVE_SYSTEMC
	{
		HLS_DEFINE_PROTOCOL("input");
		array = i_rgb.get();
		wait();
	}	
#else
	array = i_rgb.read();
#endif
	MSB = 7;
	LSB = 0;
	
	for (int i = 0; i < 16; i++) {
		A[i] = array.range(MSB, LSB);		
		MSB += 8;
		LSB += 8;
	}
```
```cpp
	// output
	sc_dt::sc_biguint<128> result;
	MSB = 7;
	LSB = 0;
	
	for (int i = 0; i < 16; i++) {
		result.range(MSB, LSB) = C[i];
		MSB += 8;
		LSB += 8;
	}
	
#ifndef NATIVE_SYSTEMC
	{
		HLS_DEFINE_PROTOCOL("output");
		o_result.put(result);
		wait();
	}
#else
	o_result.write(result);
#endif
```
一開始把資料讀進來後回存到 `A` ， 中間 sorting 的結果會存到 `C` 最後再把 `C` 儲存的值輸出。

```cpp
// sorting
for (int w = 1; w < 16; w = w * 2) {
    // make pair
    for (int first = 0; first < 16; first += 2 * w) {
        int second = first + w;
        int end = second + w - 1;
        int l1 = first, l2 = second;
        int index = first;
        // merge
        for (int i = 0; i < 2 * w; i++) {
            if (l2 <= end && (A[l1] > A[l2] || l1 >= second)) {
                C[index++] = A[l2++];
            } else {
                C[index++] = A[l1++];
            }
        }
    }

    for (int i = 0; i < 16; i++) {
        A[i] = C[i];
    }
}
```
最後是 sorting 部分。

在 BASIC 版本中有一個缺點是 `A`, `C` 被 map 到一般的 memory 沒有做 flatten 或是 map 到 register，因此在通一時間裡只能讀寫一個值。 因此我針對這點做一點改進，我把 `A`, `C` map 到 register bank。

|      | BASIC | BASIC + reg |
| ---- | ----- | ----------- |
| time | 4630  | 3040        |
| area | 14426 | 7176        |

時間上如預期，減少了不少。除此之外面積也降低了很多，可能是因為讀取變得更簡單，少了很多控制。

### unroll
接下來我針對外部兩個迴圈做 unroll。
![](https://i.imgur.com/xHLr54j.png)

當時我預想的結果會像下圖，merge 的部分可以平行做，雖然對於同個 W 而言，merge 之間不存在 dependency 但是因為寫法的關係 stratus 判定為有 dependency 所以實質上沒有完全的平行做。
![](https://i.imgur.com/GWiWvXG.png)

|      |  unroll (reg) | BASIC (reg)|
| ---- |  -----  | ----------- |
| time |  2580   | 3040        |
| area |  7810   | 7176        |

從上方的結果可以從時間發現時間只有進步一點點，是因為並不如所想一樣，完全展開。

### pipeline
![](https://i.imgur.com/6o8lCXu.png)
接下來我做的嘗試是在 merge 的地方做 pipeline。由於我在最裡面做了 pipeline 所以最外面兩層的 unroll 會被忽略。

最後出來的結果比原本 unroll 兩層還要更好。
原因是因為 unroll 的 dependency 太多，導致效果不好，而且 pipeline 的地方是整個演算法重複最多次的地方，因次效果顯著提升。

|      | unroll (reg) | pipeline (reg) |
| ---- | ------------ | -------------- |
| time | 2580         | 700            |
| area | 7810         | 8349           |

## version 2 (multiple stage)
原本的硬體在第一個 input 還沒算出來之前都不能在吃新的 input 近來，但是很明顯的是前面使用的硬體是可以先給下一筆數據使用的，所以這個版本把原本的 merge sort 切成 4 個 stage，這幾個 stage 可以各自處理不同筆的數據。

![](https://i.imgur.com/2vBgpwp.png)

![](https://i.imgur.com/yMZoB6D.png)
如上圖所示，平均每 200ns 就可以往前推一個 stage，處理 5 筆數據需要 1590ns ， 相比於 version 1 的 pipeline 版本它需要 3500ns。

但這個版本還是有缺點的，因為每個 stage 同時在處理不同筆的數據，所以各自都有自己的 register 來儲存 input 資料和要 output 的結果。這造成需要額外的的面積。還有在做 input 跟 output 的時候都要額外的 `wait()` ，在 1 個 stage 的版本中 I/O 只要做一次，但是在 4 個 stage 的版本中對於同一筆數據他要經過 4 組的 I/O。
![](https://i.imgur.com/98DPNS2.png)





