@echo off


if exist "C:\Program Files (x86)\Google\Chrome\Application\chrome.exe" (
	"C:\Program Files (x86)\Google\Chrome\Application\chrome.exe" --app="file://%~dp0html/index.html"
) else (
	"C:\Program Files\Google\Chrome\Application\chrome.exe" --app="file://%~dp0html/index.html"
)



