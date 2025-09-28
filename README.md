# Web App for WaveShare 3.52" B E-Paper Screen  

An open-source web app built for **ESP32/ESP8266** with a WaveShare 3.52" B e-paper display (SPI).  
It lets you:  
- Configure Wi-Fi or AP mode  
- Convert PNG/JPEG images into a screen-ready format  
- Update firmware easily with ElegantOTA  

This started as a **side project for my graduation**, and I decided to share it here.  

---

## ✨ Features  
- 🖼️ Image processing for WaveShare 3.52" B e-paper  
- 📡 Wi-Fi Access Point setup with editable password  
- 🔄 Over-the-air updates using ElegantOTA  

---

## ⚙️ Configuration  
- Default AP password: `12345678`  
- To change it:  
  - Open `WifiManager.ino`  
  - Update the value under **AP Password**  

---

## 📦 Hardware Used  
- [WaveShare 3.52" B e-paper](https://ar.aliexpress.com/item/1005007875624220.html?spm=a2g0o.order_list.order_list_main.5.3a2a1802e5jjeg&gatewayAdapt=glo2ara)  
- [Lithium battery charging board](https://ar.aliexpress.com/item/1005008134402242.html?spm=a2g0o.order_list.order_list_main.20.3a2a1802e5jjeg&gatewayAdapt=glo2ara)  
- [ESP32 dev board](https://ar.aliexpress.com/item/1005001408165904.html?spm=a2g0o.order_list.order_list_main.60.3a2a1802e5jjeg&gatewayAdapt=glo2ara)  
- [3.7V 250mAh lithium battery](https://ar.aliexpress.com/item/1005007707591279.html?spm=a2g0o.order_list.order_list_main.45.3a2a1802e5jjeg&gatewayAdapt=glo2ara)  

---

## 🔋 Battery Notes  
One limitation I ran into:  
- If the battery drains completely, it **must be recharged through the charging board** (the ESP can’t bring it back on its own).  
- There’s currently **no way to limit discharge** with this setup.  
- The software also can’t measure the **remaining battery capacity**.  

---

## 🖼️ How Image Processing Works  
There are **four processing modes**:  
- Threshold Mono  
- Threshold Color  
- Dither Mono  
- Dither Color  

For dithering, I used **Atkinson error-diffusion**.  
Once processed, image is split into **two 1-bit images (black and red)** before being sent to the screen.  

---

## 📂 3D Printable Case  
A simple **3D-printable case** for the screen and components is also in the works.  
I’ll share the files on **Printables** soon.  

---

## 🚀 Installation  
1. Open the project in **Arduino IDE**  
2. Build and upload it to your ESP32/ESP8266  
3. Hook up the WaveShare 3.52" B e-paper display via SPI  

---

## 📖 Usage  
1. Power on the ESP32/ESP8266  
2. Connect to the AP:  
   - **SSID**: `e-paper-XXXXXX` (last 6 digits of the MAC address)  
   - **Password**: `12345678` (default)  
3. Open the **Settings** page to configure Wi-Fi  
4. Use the **Upload Image** page to process and display PNG/JPEG images  

---

## 📸 Screenshots  
![Upload](https://res.cloudinary.com/dnfii0una/image/upload/v1759076246/e-paper-3_zyd0gn.png)
![Home](https://res.cloudinary.com/dnfii0una/image/upload/v1759076245/e-paper-1_occ8lr.png) 
![Screen](https://res.cloudinary.com/dnfii0una/image/upload/v1759076247/e-paper-4_udvfjb.png) 
![Internals](https://res.cloudinary.com/dnfii0una/image/upload/v1759076248/e-paper-6_tmxlma.png) 



---

## 🛠️ Tech Stack  
- ESP32 / ESP8266  
- Arduino IDE  
- ElegantOTA  
- WaveShare 3.52" B e-paper (SPI)  

---

## 📄 License  
This project is under the **MIT License** – use it, modify it, or share it freely.  
