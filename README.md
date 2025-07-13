# 🌿 Smart Urban Air Quality Monitoring Network

A low-cost, scalable air quality monitoring system using ESP32, PMS7003, MQ135, BME680 sensors with real-time automation via n8n. The system sends live AQI readings, logs data to Google Sheets, triggers weekly heatmaps, and sends pollution alerts via Telegram/WhatsApp.

---

## 📌 Features

- Real-time AQI and environmental data via ESP32
- Logs data to **Google Sheets** using n8n
- Sends **alerts to Telegram/WhatsApp** if AQI > 150
- **Weekly heatmap generation** trigger using Mapbox
- Dashboard ready: Node-RED / Grafana / Web UI
- Fully open-source and modular

---

## 🧰 Tech Stack

| Layer | Tools Used |
|-------|------------|
| Hardware | ESP32, PMS7003, MQ135, BME680 |
| Firmware | Arduino IDE, C++ |
| Automation | n8n (self-hosted or cloud), Webhooks |
| Alerts | Telegram Bot API, WhatsApp (via Twilio or Gupshup) |
| Data Storage | Google Sheets |
| Visualization | Node-RED / Mapbox / Grafana |
| Forecasting (optional) | Python, Pandas, Scikit-learn |

---
