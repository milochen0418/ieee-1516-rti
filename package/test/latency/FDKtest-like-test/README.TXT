

本程式將FDK上的drti-latency。由NG版本改為 IEEE 1516版本

本程式不僅單純只是個測試程式。


source code的部份：
同時透過本程式的study，也能掌握出NG 版本跟 IEEE 1516寫法兩者的不一樣之處。

對於要把NG改成IEEE 1516的 code，我會先用 /* ... */ 把NG的部份註解掉，緊接著

在後面加上的code，便是由NG 改成 IEEE 1516的code。



FDDfile的部份：
本資料夾將原來FDK專用的 fedfile3 改成 "sample.xml"，作為 IEEE 1516 的 FED File

本資料夾存放有原來的fedfile3，因此比對fedfile3與sample.xml之後，可以學習如何

將NG的FED改寫成IEEE 1516的FED


arch.h的部份：
在FDK的測試程式有使用到非NG規範下的函數，而這函數又是FDK內部實踐專屬用的函數。

而arch.h是實踐 FDK RTI的其中一個環節。

故我將FDK測試程式裡會使用到 arch.h 的部份，獨立抽取出來成為 FDK_arch.h。

以保持測試程式與先FDK 上 drti-latency 程式的等價性。


本程式雖然由drti-latency更改過來的，不過它的執行確是不正常的，會進入無限迴圈，
至於原因的話，則還要再查才有辦法理的清楚。
