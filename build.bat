@echo off
echo Building Expense Tracker with Emscripten...
emcc main.c -o index.html --shell-file shell.html -s ASYNCIFY=1 -s EXPORTED_RUNTIME_METHODS="['stringToUTF8']"
echo Build complete! 
echo Run a local web server to test it. Example: python -m http.server 8000
echo Once deployed, GitHub Pages will host it directly.
pause
