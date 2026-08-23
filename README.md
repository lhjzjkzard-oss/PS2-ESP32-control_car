# PS2 乒乓球撿球車 / PS2 Ping-Pong Ball Pickup Car

用 ESP32-C3 + PS2 手把做出你的第一臺遙控撿球車！
Build your first remote-control pickup car with an ESP32-C3 and a PS2 controller!

- `PS2X_Example_ESP32` — 主課程用：左搖桿 = 左輪、右搖桿 = 右輪，L1 = 撿球滾刷
  Main workshop sketch: left stick = left wheel, right stick = right wheel, L1 = pickup roller
- `PS2X_RightStick_ESP32` — 進階挑戰：單右搖桿開車（Y 前進、X 轉向），左搖桿控制滾刷
  Advanced challenge: single right-stick drive (Y = throttle, X = steering), left stick controls the roller

---

## 一、下載程式碼 / Download the Code

### 方法一：直接下載單一檔案（最簡單）/ Method 1: Download a single file (easiest)

1. 點上面的 `PS2X_Example_ESP32.ino` 檔案
   Click the `PS2X_Example_ESP32.ino` file above.
2. 右上角點「下載」按鈕（Download raw file）
   Click the download button in the top-right corner (Download raw file).
3. 存到桌面
   Save it to your desktop.

> Arduino IDE 開啟這個檔案時會問「要放進同名資料夾嗎？」→ 選**確定**就好
> When opening the file, the Arduino IDE will ask to create a folder for it → click **OK**.

### 方法二：下載整包 ZIP / Method 2: Download the whole repo as ZIP

1. 頁面右上角綠色 `Code` 按鈕 → `Download ZIP`
   Click the green `Code` button → `Download ZIP`.
2. 解壓縮 → 打開 `PS2X_Example_ESP32.ino`
   Unzip it, then open `PS2X_Example_ESP32.ino`.

---

## 二、課前安裝（照順序做，約 30 分鐘）/ Setup (do in order, ~30 minutes)

1. **安裝 Arduino IDE**：到 arduino.cc 下載安裝
   **Install the Arduino IDE**: download it from arduino.cc.
2. **安裝 ESP32 板子套件**：
   **Install the ESP32 board package**:
   - 檔案 → 偏好設定 → 額外的開發板管理員網址，貼上：
     File → Preferences → Additional boards manager URLs, paste:
     `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
   - 工具 → 開發板 → 開發板管理員 → 搜尋 `esp32` → 安裝（Espressif Systems 出品）
     Tools → Board → Boards Manager → search `esp32` → install (by Espressif Systems).
3. **安裝 PS2X 函式庫**（手動安裝，它不在內建程式庫管理員裡）：
   **Install the PS2X library** (manual install; it is not in the built-in Library Manager):
   - 到 https://github.com/madsci1016/Arduino-PS2X → 綠色 `Code` → `Download ZIP`
     Go to https://github.com/madsci1016/Arduino-PS2X → green `Code` → `Download ZIP`.
   - 解壓縮，找到裡面的 `PS2X_lib` 資料夾（是**裡面那層**，不是最外層）
     Unzip it and find the `PS2X_lib` folder inside (the **inner** folder, not the top-level one).
   - 把 `PS2X_lib` 資料夾複製到 Arduino 的 libraries 資料夾：
     Copy the `PS2X_lib` folder into the Arduino libraries folder:
     - Mac：文件（Documents）→ Arduino → libraries
       Mac: Documents → Arduino → libraries
     - Windows：文件 → Arduino → libraries
       Windows: Documents → Arduino → libraries
   - 關掉 Arduino IDE 再重開
     Restart the Arduino IDE.

---

## 三、燒錄步驟 / Flash the Sketch

1. USB 線接開發板（先不要接其他線）
   Connect the board with a USB cable (do not wire anything else yet).
2. 開 Arduino IDE → 開啟 `PS2X_Example_ESP32.ino`
   Open the Arduino IDE → open `PS2X_Example_ESP32.ino`.
3. 工具 → 開發板 → `ESP32C3 Dev Module`
   Tools → Board → `ESP32C3 Dev Module`.
4. **工具 → USB CDC On Boot → `Enabled`**（重要！沒開右輪不會動）
   **Tools → USB CDC On Boot → `Enabled`** (important! The right wheel will not work without it).
5. 工具 → 序列埠 → 選出現的那個
   Tools → Port → select the port that appears.
6. 按「上傳」（→ 箭頭），等到下方出現 `Done uploading`
   Click Upload (→), wait for `Done uploading`.
7. 工具 → 序列埠監控視窗 → 右下角選 `115200`
   Tools → Serial Monitor → select `115200` at the bottom-right.
8. 看到 `#try config 1` 一直重複 = **成功！**（接收器還沒接，車子在等它，這是正常的）
   Seeing `#try config 1` repeat = **success!** (The receiver is not wired yet; the car is waiting for it. This is normal.)

> 卡在 `Connecting...` 不動 → 按一下開發板上的 RST 按鈕
> Stuck at `Connecting...` → press the RST button on the board once.

---

## 四、接線表 / Wiring

```
PS2 接收器 / receiver： CLK→GPIO4  CMD→GPIO5  SEL→GPIO6  DAT→GPIO7
                        VCC→3.3V   GND→GND        ← 3.3V！接 5V 會燒掉 / 3.3V only! 5V will kill it

左輪 / left wheel (L298N)：IN1→GPIO0  IN2→GPIO1
右輪 / right wheel (L298N)：IN3→GPIO20 IN4→GPIO21
                        GND→GND（共地！/ common ground!）  電源/power→電池盒/battery pack
                        ENA/ENB 跳帽插著 / keep the ENA/ENB jumpers on

撿球滾刷 / pickup roller (M3)：訊號/signal→GPIO3  VCC→電池/battery  GND→GND（共地！/ common ground!）
```

**共地是什麼？** 訊號是「相對電壓」，馬達模組和開發板的 GND 一定要接在一起，不然馬達不聽話。
**What is common ground?** Signals are relative voltages — the GND of every module must be connected to the board's GND, or the motors will not respond.

---

## 五、操作方式（主課程版）/ Controls (main sketch)

| 操作 / Control | 動作 / Action |
|------|------|
| 左搖桿 / Left stick | 控制左輪（推 = 前轉、拉 = 反轉）/ Left wheel (up = forward, down = reverse) |
| 右搖桿 / Right stick | 控制右輪 / Right wheel |
| 兩支一起推 / Both sticks up | 車子前進 / Car moves forward |
| 按住 L1 / Hold L1 | 撿球滾刷轉動 / Pickup roller spins |
| 放開 L1 / Release L1 | 滾刷停止 / Roller stops |

開機順序：**先開手把**（紅燈亮）→ 接電池 → 按開發板 RST 按鈕。
Power-on order: **turn on the controller first** (red LED on) → connect the battery → press RST on the board.

---

## 六、兩個版本的差別 / Differences Between the Two Sketches

| | 雙搖桿版（主課程）/ Dual-stick (main) | 右搖桿版（挑戰）/ Right-stick (challenge) |
|--|-------------------|-----------------|
| 開車方式 / Driving | 左搖桿=左輪、右搖桿=右輪 / Left stick = left wheel, right stick = right wheel | 右搖桿一支開車（Y 前進、X 轉向）/ Single right stick (Y = throttle, X = steering) |
| 滾刷 / Roller | L1 按鈕開/關（GPIO3 一條線）/ L1 on/off (single wire on GPIO3) | 左搖桿調速正反轉（GPIO3 + GPIO10 兩條線）/ Left stick, bidirectional speed (two wires: GPIO3 + GPIO10) |

---

## 七、課後挑戰 / Challenges

1. **入門 / Beginner**：把 `* 2` 改成 `* 3` → 車子變快還是變慢？
   Change `* 2` to `* 3` → is the car faster or slower?
2. **入門 / Beginner**：把死區 `20` 改成 `50` → 搖桿要推更多車子才動
   Change the deadzone `20` to `50` → the stick must be pushed further before the car moves.
3. **進階 / Advanced**：燒錄 `PS2X_RightStick_ESP32.ino`（單搖桿開車版，M3 要多接一條線到 GPIO10）
   Flash `PS2X_RightStick_ESP32.ino` (single-stick drive; M3 needs one extra wire to GPIO10).
4. **魔王 / Expert**：把 M3 的 L1 改成別的按鈕（提示：改 `PSB_L1`，可換成 `PSB_CIRCLE`）
   Change M3's L1 to another button (hint: change `PSB_L1`, e.g. to `PSB_CIRCLE`).

---

## 八、原始碼來源與授權 / Source & License

本專案程式碼改寫自 [Arduino-PS2X](https://github.com/madsci1016/Arduino-PS2X) 函式庫的範例程式：
This project is based on the example sketch from the [Arduino-PS2X](https://github.com/madsci1016/Arduino-PS2X) library:

- 函式庫作者 / Library author：[Bill Porter](http://www.billporter.info/)
- 最早的 PS2 控制器程式碼 / Original PS2 controller code：Shutter（Arduino 論壇 / Arduino Forum）
- 本專案新增 / Added in this project：ESP32-C3 腳位設定、L298N 馬達控制、撿球滾刷控制
  ESP32-C3 pin mapping, L298N motor control, pickup roller control

原範例為 **GPL v3** 授權，因此本專案同樣以 **GPL v3** 釋出（見 LICENSE 檔案）。
The original example is licensed under **GPL v3**, so this project is also released under **GPL v3** (see the LICENSE file).
