# GUI-based-Lock-System

### **Project Title**: Keypad Lock System with Sound Feedback and LCD Display

---

### **Project Description**
This project implements a **Keypad Lock System** using **PSoC 5LP**, designed to secure access with a 4-digit PIN. The system integrates several components, including a **PWM-based buzzer for sound feedback**, an **LCD for user interaction**, and an **EEPROM for password storage**. The system also features USB communication via the **USBUART module** for debugging and feedback.

---

### **Features**
1. **Keypad Input**:
   - Users can input a 4-digit PIN using a keypad.
   - Supports commands like "Enter," "Cancel," and "New Pass."

2. **LCD Display**:
   - Displays system status (e.g., "LOCKED," "UNLOCKED").
   - Provides feedback during password entry and changes.

3. **Sound Feedback**:
   - A PWM-based buzzer provides sound effects:
     - Error tone for incorrect PIN entry.
     - Success tone for correct PIN entry or successful operations.

4. **EEPROM Integration**:
   - Stores the 4-digit password persistently.
   - Default password is set to "1234" if no password exists.

5. **USB Communication**:
   - USBUART module enables serial communication for debugging and feedback.
   - Sends responses such as "System Unlocked," "Wrong Password," or "New Password Set."

6. **Lockout Mechanism**:
   - After three consecutive incorrect PIN entries, the system locks out for 30 seconds, displaying a countdown on the LCD.

---

### **Hardware Components**
1. **PSoC 5LP Microcontroller**
2. **Character LCD (16x2)**
3. **Piezoelectric Buzzer**
4. **Keypad (4x3 or 4x4 matrix)**
5. **LED Indicator**
6. **USB Interface**
7. **Emulated EEPROM**

---

### **Software Components**
1. **PWM Component**:
   - Configured in UDB mode with an 8-bit resolution.
   - Generates tones for sound feedback.

2. **Clock Component**:
   - Provides a 500 kHz clock source for the PWM module.

3. **LCD Component**:
   - Displays system messages and guides users during operations.

4. **USBUART Component**:
   - Facilitates serial communication for debugging and external control.

5. **EEPROM Component**:
   - Stores the password persistently across power cycles.

---

### **How It Works**
1. The user enters a 4-digit PIN using the keypad.
2. The system compares the entered PIN with the stored password in EEPROM.
3. Based on the result:
   - If correct: Plays a success tone, unlocks the system, and displays "UNLOCKED" on the LCD.
   - If incorrect: Plays an error tone, increments the wrong attempt counter, and displays "Wrong PIN."
4. After three consecutive wrong attempts, the system locks out for 30 seconds with a countdown displayed on the LCD.
5. Users can change the password by entering the current password first, followed by the new one.

---

### **Configuration Details**
#### PWM Configuration:
- Implementation: UDB
- Resolution: 8-bit
- Period: 255
- Compare Value: 127 (50% duty cycle)
- Clock Frequency: 500 kHz

#### Clock Configuration:
- Frequency: 500 kHz

#### EEPROM Configuration:
- Address: `0x00`
- Default Password: `"1234"`

---

### **Usage Instructions**
1. Connect all hardware components as per the schematic.
2. Load the firmware onto the PSoC device using PSoC Creator.
3. Use a terminal application to monitor USBUART communication (e.g., PuTTY).
4. Interact with the system via the keypad and observe feedback on the LCD and buzzer.

---

### **Future Enhancements**
1. Add support for multiple user accounts with separate passwords.
2. Implement wireless communication (e.g., Bluetooth) for remote control.
3. Enhance security by introducing encryption for stored passwords.
4. Add real-time clock functionality to log access timestamps.

---

### **Repository Structure**
```
/KeypadLockSystem
├── Firmware/
│   ├── main.c
│   ├── project.cydsn/
│   └── TopDesign.cysch
├── Hardware/
│   ├── Schematic.png
│   └── PCB_Layout.png
├── README.md
└── LICENSE
```

This project demonstrates how to combine multiple PSoC components to create a functional and interactive security system suitable for educational purposes or small-scale applications!

Citations:
[1] https://pplx-res.cloudinary.com/image/upload/v1744363871/user_uploads/lcHWmIUwevmewJJ/image.jpg
[2] https://pplx-res.cloudinary.com/image/upload/v1744363934/user_uploads/yCySMHAgixnBHQQ/image.jpg
[3] https://pplx-res.cloudinary.com/image/upload/v1744364205/user_uploads/PMDMIMCQxreIwpn/image.jpg

---
Answer from Perplexity: pplx.ai/share
