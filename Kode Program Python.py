from ultralytics import YOLO
import cv2
import cvzone
import time
import serial

# Buka dua webcam dengan indeks 0 dan 1
cap1 = cv2.VideoCapture(0)  # Webcam pertama
cap2 = cv2.VideoCapture(3)  # Webcam kedua

cap1.set(3, 640)  # Atur lebar frame (opsional)
cap1.set(4, 360)  # Atur tinggi frame (opsional)

cap2.set(3, 640)  # Atur lebar frame (opsional)
cap2.set(4, 360)  # Atur tinggi frame (opsional)

#cap = cv2.VideoCapture("../Videos/motorbikes.mp4")  # For Video

ser = serial.Serial('COM6', 9600)

model = YOLO("../Yolo-Weights/AmbulansV2.8s.pt")

classNames = ["Ambulans Keluar", "Ambulans Masuk"]

last_led_on_time = 0  # Waktu terakhir perintah "LED_HIDUP" dikirim
last_led_off_time = 0  # Waktu terakhir perintah "LED_MATI" dikirim

while True:
    pross1, img1 = cap1.read()
    pross2, img2 = cap2.read()

    if pross1 and pross2:
        # Ganti ukuran frame jika perlu

        # Gunakan OpenCV untuk menggabungkan kedua frame secara horizontal
        combined_img = cv2.vconcat([img1, img2])
        results = model(combined_img, stream=True)
        for r in results:
            boxes = r.boxes
            for box in boxes:
                # Bounding Box
                x1, y1, x2, y2 = box.xyxy[0]
                x1, y1, x2, y2 = int(x1), int(y1), int(x2), int(y2)
                # cv2.rectangle(img,(x1,y1),(x2,y2),(255,0,255),3)
                w, h = x2 - x1, y2 - y1
                cvzone.cornerRect(combined_img, (x1, y1, w, h), l=30, t=5, rt=1,
                                  colorR=(0, 255, 0), colorC=(0, 255, 255))
                # Confidence
                conf = f'{int(box.conf[0] * 100)}%'
                # Class Name
                cls = int(box.cls[0])
                currentClass = classNames[cls]

                if currentClass == "Ambulans Masuk" and int(box.conf[0] * 100) > 70:
                    current_time = time.time()
                    # Periksa apakah sudah 1 detik sejak perintah terakhir dikirim
                    if current_time - last_led_on_time >= 5:
                        command = "ON"
                        ser.write(command.encode())
                        response = ser.readline()
                        last_led_on_time = current_time  # Perbarui waktu terakhir perintah
                        # print("Perintah 'LED_HIDUP' dikirim")
                elif currentClass == "Ambulans Keluar" and int(box.conf[0] * 100) > 70:
                    current_time = time.time()
                    # Periksa apakah sudah 1 detik sejak perintah terakhir dikirim
                    if current_time - last_led_off_time >= 5:
                        command = "OFF"
                        ser.write(command.encode())
                        response = ser.readline()
                        last_led_off_time = current_time  # Perbarui waktu terakhir perintah
                        # print("Perintah 'LED_HIDUP' dikirim")

                cvzone.putTextRect(combined_img, f'{classNames[cls]} {conf}', (max(0, x1), max(35, y1)),
                                   scale=1, thickness=1, colorT=(0, 0, 0), colorR=(100, 255, 255))


        cv2.imshow("Image", combined_img)
    cv2.waitKey(1)
