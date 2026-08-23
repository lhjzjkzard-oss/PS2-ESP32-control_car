# PS2 乒乓球撿球車

用 ESP32-C3 + PS2 手把做出你的第一臺遙控撿球車！

- `PS2X_Example_ESP32` — 主課程用：左搖桿 = 左輪、右搖桿 = 右輪，L1 = 撿球滾刷
- `PS2X_RightStick_ESP32` — 進階挑戰：單右搖桿開車（Y 前進、X 轉向），左搖桿控制滾刷

---

## 一、下載程式碼（兩種方法）

### 方法一：直接下載單一檔案（最簡單）

1. 點進上面的 `PS2X_Example_ESP32` 資料夾
2. 點 `PS2X_Example_ESP32.ino`
3. 右上角點「下載」按鈕（Download raw file）
4. 存到桌面

> Arduino IDE 開啟這個檔案時會問「要放進同名資料夾嗎？」→ 選**確定**就好

### 方法二：下載整包 ZIP

1. 頁面右上角綠色 `Code` 按鈕 → `Download ZIP`
2. 解壓縮 → 打開 `PS2X_Example_ESP32` 資料夾裡的 .ino 檔

---

## 二、課前安裝（照順序做，約 30 分鐘）

1. **安裝 Arduino IDE**：到 arduino.cc 下載安裝
2. **安裝 ESP32 板子套件**：
   - 檔案 → 偏好設定 → 額外的開發板管理員網址，貼上：
     `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
   - 工具 → 開發板 → 開發板管理員 → 搜尋 `esp32` → 安裝（Espressif Systems 出品）
3. **安裝 PS2X 函式庫**（手動安裝，它不在內建程式庫管理員裡）：
   - 到 https://github.com/madsci1016/Arduino-PS2X → 綠色 `Code` → `Download ZIP`
   - 解壓縮，找到裡面的 `PS2X_lib` 資料夾（是**裡面那層**，不是最外層）
   - 把 `PS2X_lib` 資料夾複製到 Arduino 的 libraries 資料夾：
     - Mac：文件（Documents）→ Arduino → libraries
     - Windows：文件 → Arduino → libraries
   - 關掉 Arduino IDE 再重開

---

## 三、燒錄步驟

1. USB 線接開發板（先不要接其他線）
2. 開 Arduino IDE → 開啟 `PS2X_Example_ESP32.ino`
3. 工具 → 開發板 → `ESP32C3 Dev Module`
4. **工具 → USB CDC On Boot → `Enabled`**（重要！沒開右輪不會動）
5. 工具 → 序列埠 → 選出現的那個
6. 按「上傳」（→ 箭頭），等到下方出現 `Done uploading`
7. 工具 → 序列埠監控視窗 → 右下角選 `115200`
8. 看到 `#try config 1` 一直重複 = **成功！**（接收器還沒接，車子在等它，這是正常的）

> 卡在 `Connecting...` 不動 → 按一下開發板上的 RST 按鈕

---

## 四、接線表

```
PS2 接收器： CLK→GPIO4  CMD→GPIO5  SEL→GPIO6  DAT→GPIO7
             VCC→3.3V   GND→GND        ← 3.3V！接 5V 會燒掉

左輪(L298N)：IN1→GPIO0  IN2→GPIO1
右輪(L298N)：IN3→GPIO20 IN4→GPIO21
             GND→GND（共地！）  電源→電池盒  ENA/ENB 跳帽插著

撿球滾刷(M3)：訊號→GPIO3  VCC→電池  GND→GND（共地！）
```

**共地是什麼？** 訊號是「相對電壓」，馬達模組和開發板的 GND 一定要接在一起，不然馬達不聽話。

---

## 五、操作方式（主課程版）

| 操作 | 動作 |
|------|------|
| 左搖桿 | 控制左輪（推 = 前轉、拉 = 反轉） |
| 右搖桿 | 控制右輪 |
| 兩支一起推 | 車子前進 |
| 按住 L1 | 撿球滾刷轉動 |
| 放開 L1 | 滾刷停止 |

開機順序：**先開手把**（紅燈亮）→ 接電池 → 按開發板 RST 按鈕。

---

## 六、兩個版本的差別

| | 雙搖桿版（主課程） | 右搖桿版（挑戰） |
|--|-------------------|-----------------|
| 開車方式 | 左搖桿=左輪、右搖桿=右輪 | 右搖桿一支開車（Y 前進、X 轉向） |
| 滾刷 | L1 按鈕開/關（GPIO3 一條線） | 左搖桿調速正反轉（GPIO3 + GPIO10 兩條線） |

---

## 七、課後挑戰

1. **入門**：把 `* 2` 改成 `* 3` → 車子變快還是變慢？
2. **入門**：把死區 `20` 改成 `50` → 搖桿要推更多車子才動
3. **進階**：燒錄 `PS2X_RightStick_ESP32`（單搖桿開車版，M3 要多接一條線到 GPIO10）
4. **魔王**：把 M3 的 L1 改成別的按鈕（提示：改 `PSB_L1`，可換成 `PSB_CIRCLE`）

---

## 八、原始碼來源與授權

本專案程式碼改寫自 [Arduino-PS2X](https://github.com/madsci1016/Arduino-PS2X) 函式庫的範例程式：

- 函式庫作者：[Bill Porter](http://www.billporter.info/)
- 最早的 PS2 控制器程式碼：Shutter（Arduino 論壇）
- 本專案新增：ESP32-C3 腳位設定、L298N 馬達控制、撿球滾刷控制

原範例為 **GPL v3** 授權，因此本專案同樣以 **GPL v3** 釋出（見 LICENSE 檔案）。
