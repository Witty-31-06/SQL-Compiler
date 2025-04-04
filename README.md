
# SQL Compiler & Parser

This is a C++ based SQL Parser and Compiler that takes SQL queries as input, tokenizes them, parses them using LL(1) parsing techniques, and outputs derivations and errors with rich formatted tables using the `tabulate` library. It also reads grammar from a JSON file and dynamically constructs the parsing table.

---

## 🧰 Features

- **Lexer**: Tokenizes SQL queries  
- **Parser**: LL(1) table-based parser  
- **Custom Grammar Support**: Load grammar from `grammar.json`  
- **Verbose Debugging**: With levels 1 and 2  
- **Formatted Output**: Using `tabulate` and syntax highlighting  
- **Cross-platform**: Buildable on both Linux and Windows using CMake  

---

## 📸 Demo

<table>
<tr>
<td><b>Parsing Table in Action</b></td>
<tr>
<td><img src="demo/demo1.gif" width="600"/></td>
</tr>
</table>


---

## 🚀 Getting Started

### 🐧 Linux Instructions

#### 1. Clone the Repository

```bash
git clone https://github.com/your-username/sql-compiler.git
cd sql-compiler
```

#### 2. Install Dependencies

```bash
sudo apt update
sudo apt install cmake g++ libfmt-dev
```

> Make sure you're using **C++17** or newer.

#### 3. Build the Project

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

#### 4. Run the Executable

```bash
./SQLParser -v 1
```

---

### 🪟 Windows Instructions (VS 2022)

#### 1. Install Tools

- [Visual Studio 2022](https://visualstudio.microsoft.com/vs/)
  - Install "Desktop Development with C++"
- [CMake for Windows](https://cmake.org/download/)

#### 2. Open Project

- Open **CMakeLists.txt** in Visual Studio
- Configure and generate with desired compiler (e.g., MSVC)
- Build the target `SQLParser`

#### 3. Run with Arguments

In VS:  
- Right click the project → Properties  
- Go to Debugging → Command Arguments:  
  `--verbose 2`

---

## ⚙️ Command-Line Usage

```bash
./SQLParser [options]

Options:
  -v, --verbose [1|2]    Set verbosity level (default: 1)
  -h, --help             Show this help message
```

---

## 📁 Project Structure

```
.
├── CMakeLists.txt
├── grammar.json
├── src/
│   ├── Lexer.cpp/h
│   ├── Token.cpp/h
│   ├── parser.cpp/h
│   ├── SQL_Interpreter.cpp/h
│   └── Interpreter_main.cpp
├── demo/
│   └── parsing.gif
```

---

## 👨‍💻 Contributors

| Name | GitHub |
|------|--------|
| Siddhartha Mondal | [@witty-31-06](https://github.com/witty-31-06) |
| Soumyadeep Singh | [@singhsoumya0109](https://github.com/singhsoumya0109) |
| Sugata Laha | [@sugatalaha](https://github.com/sugatalaha) |
| Subham Mondal | [@Subham-Mondal-973](https://github.com/Subham-Mondal-973) |


---


### 📦 Special Thanks

This project wouldn't have been possible without the help of these amazing open-source libraries:

- 🧩 [**nlohmann/json**](https://github.com/nlohmann/json) – for elegant and easy-to-use JSON parsing in C++.
- 📊 [**tabulate**](https://github.com/p-ranav/tabulate) – for beautifully formatted and customizable tables in the terminal.

Huge shoutout to their contributors and maintainers! 💙

---
