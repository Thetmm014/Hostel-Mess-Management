# Hostel Mess Management System 🍽️

A C++ based hostel mess management system that handles student meal tracking, attendance, monthly billing, complaints and feedback — built using Object Oriented Programming concepts with file based persistent storage.

---

## 🚀 Features

### Manager
- Add new students with auto-generated roll numbers and room numbers
- Mark attendance per session (morning / afternoon / evening)
- Three eating modes per session:
  - Base meal only
  - Base meal + extras
  - Extras only (paying on spot, no base charge)
- View individual student bills
- Export monthly billing report as CSV
- Manage extras menu (add / delete / update prices)
- Update base meal prices per session
- View student complaints
- Student checkout with final bill and record preservation

### Student
- Login with roll number
- Forgot roll number? Recover using room number
- View own monthly bill
- View today's extras menu with prices
- Register complaints
- Give feedback

---

## 🛠️ Tech Stack

- **Language:** C++
- **Standard:** C++11
- **Storage:** File based CSV (data persists across sessions)

---

## 🗂️ OOP Structure

```
Person (Base Class)
├── Student (Inherited) — roll no, room no, join date, active status
└── Manager (Inherited) — manages the whole system

ExtraItem              — represents each item in extras menu
MessSystem             — main controller, manages all operations
```

**OOP Concepts Used:**
- **Inheritance** — Student and Manager inherit from Person
- **Encapsulation** — data and methods grouped inside classes
- **Abstraction** — MessSystem hides internal complexity
- **File Handling** — persistent CSV storage

---

## ⚙️ How to Compile and Run

### Requirements
- G++ compiler (C++11 or later)

### Compile
```bash
g++ -o mess mess.cpp
```

### Run
```bash
./mess        # Linux / Mac
mess.exe      # Windows
```

---

## 📁 Files

### You provide
| File | Description |
|------|-------------|
| `mess.cpp` | Main source code |
| `menu.csv` | Default extras menu (edit as needed) |

### Auto generated on first run
| File | Description |
|------|-------------|
| `students.csv` | All student records |
| `attendance.csv` | Daily meal and attendance records |
| `complaints.csv` | Student complaints |
| `feedback.csv` | Student feedback |
| `report_YYYY-MM.csv` | Monthly billing report (exported by manager) |

---

## 📖 How It Works

### Meal Pricing
```
Base meal price  → fixed per session (morning / afternoon / evening)
Extras           → added on top of base meal
Extras only mode → student pays only for extras, no base charge
```

### Billing Logic
```
Every attendance entry stores:
  date, roll number, session, base price, extras total

Monthly bill = sum of all entries for that student in that month
Manager exports CSV report anytime with one click
```

### Roll Number Format
```
Auto assigned → M1, M2, M3 ...
Forgotten?    → Student uses room number to find roll number
```

### Student Checkout
```
Manager runs checkout for a student
→ Final bill shown
→ Student marked inactive
→ Record preserved for future reference
```

---

## 📊 Sample Output

```
========================================
     Hostel Mess Management System
========================================
 [1] Manager Login
 [2] Student Login
 [3] Exit

  Auto Roll No: M1
  Student Name: Rahul Singh
  Room Number: 101
  Set Password: ****
  Student added! Roll No: M1

  Session: morning | Base Price: Rs.40
  Extras Menu:
  [1] Egg Curry - Rs.30
  [2] Paneer - Rs.50
  Attendance marked!
  Base: Rs.40 | Extras: Rs.30 | Total: Rs.70

  Your bill for 2026-05: Rs.70
```

---

## 🔧 Default Credentials

```
Manager Password: admin123
```

> Change this in the source code before deploying.

---

## 🔮 Planned Improvements (v2.0)

- [ ] Change manager password from inside the system
- [ ] Feedback rating system (1-5 stars per meal)
- [ ] Weekly menu planning by manager
- [ ] Meal skip / absence marking in advance by student
- [ ] View complaint status and manager response
- [ ] Detailed attendance history for student
- [ ] Multiple mess support
- [ ] GUI version

---

## 👤 Author

Lovish Grover
- GitHub: [@Thetmm014](https://github.com/Thetmm014)

---

## 📄 License

This project is open source and available under the [MIT License](LICENSE).
