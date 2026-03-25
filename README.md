# Expense & Budget Tracker (WebAssembly)

This is a **Personal Finance C Project** compiled to **WebAssembly (Wasm)** using Emscripten. Designed for university submission, it features native C core logic operating smoothly within a modern browser environment.

## Features
- **Pure C Core Logic:** Menu-driven handling using struct data types, basic arrays, input validation and logic.
- **Add and Retrieve Expenses:** Categorize, date (YYYY-MM-DD), put financial amounts, and attach notes to your expenses natively. 
- **Emscripten Virtual File Storage:** Reads and writes to files exactly as plain C does by tapping into the Emscripten temporary backend memory.
- **Budgeting Limit Tracking:** Dynamic validation checking total spending patterns inside specific months.  

## Compilation Requirement
Ensure Emscripten is configured and installed first (`emsdk`). Once configured, simply compile using the script:
```bash
# Windows
build.bat

# Manual compilation
emcc main.c -o index.html --shell-file shell.html
```

## Running the WebAssembly Build Locally
Browsers naturally stop local disk files from executing active `.wasm` objects to secure you from arbitrary code flaws (CORS limitation). Therefore, do not open `index.html` simply from `file:///...`!

To view it perfectly:
1. Open a terminal to your source folder.
2. Spin up a basic web server: `python -m http.server 8000`.
3. Browse to `http://localhost:8000`.

## GitHub Pages Setup
You can host this perfectly and permanently.
1. Push `index.html`, `index.js`, and `index.wasm` up directly to a GitHub Repository.
2. On GitHub repository page: Navigate to **Settings > Pages**.
3. Point "Source Branch" sequentially to your `main` or `master` stream.
4. Save and GitHub will provide you a link serving your WebAssembly perfectly!
