---------------------------------------------------
- pic_crop.exe 使用說明
---------------------------------------------------

cmd> pic_crop.exe 輸入檔案 cropWidth cropHeight

Example:
cmd> pic_crop.exe fruit.jpg 100 100

執行後會輸出兩個檔案:
1. fruit_box.jpg	: 這是把要 crop 的區域框出來，畫在原圖上(副本)。
2. fruit_crop.jpg	: 實際框出的區域，另存的照片。

備註:
1. 程式沒有檢查大小，所以要確認 crop size 要小於 原圖。
2. 附帶的 DLL 要跟程式 pic_crop.exe 放一起。
