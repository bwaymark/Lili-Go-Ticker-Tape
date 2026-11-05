# TickerTape

A scrolling message ticker for the **LilyGO T-Display S3**. The messages live in a plain text file on a GitHub Gist, so you can change what the display says from any browser or laptop. No re-flashing, no IP addresses.

![Platform](https://img.shields.io/badge/platform-ESP32--S3-blue)
![Arduino](https://img.shields.io/badge/Arduino-IDE%202.x-teal)
![Licence](https://img.shields.io/badge/licence-MIT-green)

---

## What It Does

- Scrolls one message at a time, right to left, in large yellow text.
- Fetches your message file **on boot**, retries every **30 seconds** until it succeeds, then refreshes every **10 minutes**.
- Only fetches between messages, so the scroll never stalls mid-sentence.
- Falls back to built-in messages if WiFi or the fetch fails.
- Converts curly quotes, long dashes and ellipses to plain ones (the font is ASCII only).

---

## Hardware

- LilyGO T-Display S3 (ESP32-S3, 1.9" 320x170 display)
- USB-C cable
- A 2.4GHz WiFi network (the ESP32-S3 does not support 5GHz)

---

## Software Setup

1. Install **Arduino IDE 2.x**.
2. Add the ESP32 board package: Boards Manager, search `esp32` by Espressif, install version **2.0.14**.
3. Install the **LilyGO TFT_eSPI fork** (the standard TFT_eSPI library will not work with this board). Download it from the [LilyGO T-Display-S3 repository](https://github.com/Xinyuan-LilyGO/T-Display-S3), copy its `lib/TFT_eSPI` folder into your Arduino `libraries` folder, and restart the IDE.
4. No other libraries are needed.

### Board settings

| Setting | Value |
|---|---|
| Board | ESP32S3 Dev Module |
| Upload Speed | 921600 |
| USB Mode | Hardware CDC and JTAG |
| USB CDC On Boot | Enabled |
| CPU Frequency | 240MHz |
| Flash Mode | QIO 80MHz |
| Flash Size | 16MB (128Mb) |
| Partition Scheme | 16M Flash (3MB APP/9.9MB FATFS) |
| PSRAM | OPI PSRAM |

---

## Configuration

Everything you need to change is in **`config.h`**, next to the sketch. The files ship with placeholders, so nothing personal is stored in this repository.

### 1. Set your WiFi details

Open `config.h` and edit:

```cpp
#define WIFI_SSID      "YOUR_WIFI_SSID"
#define WIFI_PASSWORD  "YOUR_WIFI_PASSWORD"
```

- **SSID** is the network name exactly as it appears on your phone or laptop. It is case sensitive and spaces count.
- It must be a **2.4GHz** network. If your router combines 2.4 and 5GHz under one name, that is usually fine.
- Keep the quotes.

### 2. Create your message file (GitHub Gist)

1. Sign in at [gist.github.com](https://gist.github.com) (a free GitHub account is fine).
2. Give the file a name, for example `messages.txt`.
3. Type your messages, **one per line**. See `messages.example.txt` for a sample:

   ```
   # Lines starting with # are ignored
   Good morning. The kettle is on.
   Reminder: drink some water.
   ```
4. Click the arrow next to **Create public gist** and choose **Create secret gist**, then create it.

> A **secret** gist is *unlisted*, not private. Anyone with the link can read it, and the display needs to read it without logging in. Don't put anything sensitive in it.

### 3. Get the raw URL

The sketch needs the **raw** URL, and it must be the version **without a revision hash**, otherwise it will always show the first version you saved.

1. Open your gist and click the **Raw** button (top right of the file).
2. The address will look like this:

   ```
   https://gist.githubusercontent.com/USERNAME/GIST_ID/raw/REVISION_HASH/messages.txt
   ```
3. **Delete the revision hash and the slash after it**, leaving:

   ```
   https://gist.githubusercontent.com/USERNAME/GIST_ID/raw/messages.txt
   ```
4. Paste that into `config.h`:

   ```cpp
   #define MESSAGES_URL  "https://gist.githubusercontent.com/USERNAME/GIST_ID/raw/messages.txt"
   ```

Test it by pasting the URL into a browser. You should see your plain text. If you want to try the display with built-in messages only, set `MESSAGES_URL` to `""`.

Other settings in `config.h`: refresh intervals, scroll speed (`STEP_PX`, `FRAME_MS`) and text colour (`TEXT_COLOUR`).

---

## Uploading

1. Open `TickerTap/TickerTap.ino` in Arduino IDE (keep `config.h` in the same folder).
2. Select the board settings above and the correct port.
3. Click **Upload**.
4. If the board isn't detected, hold the **BOOT** button, tap **RESET**, release BOOT, then upload again.

---

## Updating Your Messages

**In the browser:** open your gist, click **Edit**, change the text, click **Update secret gist**.

**From your laptop with git:**

```bash
git clone https://gist.github.com/GIST_ID.git
cd GIST_ID
# edit messages.txt
git commit -am "Update messages"
git push
```

The display picks up changes within about 10 minutes (after the message currently scrolling finishes). GitHub's raw cache can also take a few minutes to refresh. To see a change straight away, press the RESET button on the board; it fetches on boot.

### Message file rules

- One message per line.
- Blank lines are ignored.
- Lines starting with `#` are ignored.
- Plain English characters work best. Curly quotes and dashes are converted automatically; accented letters and emoji won't display.
- Long messages work, they just scroll for longer.

---

## Troubleshooting

Open **Serial Monitor** at **115200 baud** (with USB CDC On Boot enabled). Each fetch prints a line.

| Serial output | Meaning |
|---|---|
| `Fetch: loaded N messages` | Working. |
| `Fetch: WiFi not connected` | Wrong SSID or password, or a 5GHz-only network. |
| `Fetch HTTP: 404` | Wrong URL. Check the username, gist ID and filename, and that you removed the revision hash correctly. |
| `Fetch HTTP: -1` (or other negative) | Connection or TLS failure. Check WiFi and try again. |
| `Fetch: no usable lines` | The file was empty or contained only comments. |
| Built-in messages keep showing | The first fetch failed. It retries every 30 seconds, check Serial for the reason. |
| Screen stays black | Check the board settings, particularly USB CDC On Boot and PSRAM. |
| Edit not appearing | Wait a few minutes for the cache, or press RESET. |

---

## Privacy Notes

- Don't commit real WiFi details. If you fork this and fill in `config.h`, keep your fork private, or add `config.h` to `.gitignore` and share a `config.example.h` instead.
- A secret gist is unlisted, not private.

---

## Credits

- Built for the [LilyGO T-Display S3](https://github.com/Xinyuan-LilyGO/T-Display-S3)
- Uses [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) (LilyGO fork)

## Licence

MIT. Do what you like with it.
