$name = Read-Host "Enter dir name"
Copy-Item 'tutorial/Demo' -Destination "tutorial/$($name)"
Add-Content './tutorial/CmakeLists.txt' "`nadd_subdirectory($($name))"
CD tutorial/$name

Copy-Item '../Demo/Demo.cpp' -Destination "$($name).cpp"
Copy-Item '../Demo/Demo.h' -Destination "$($name).h"
Copy-Item '../Demo/main.cpp' -Destination "main.cpp"
Copy-Item '../Demo/InputManager.h' -Destination "InputManager.h"
Copy-Item '../Demo/CmakeLists.txt' -Destination "CmakeLists.txt"

((Get-Content -path './CmakeLists.txt' -Raw) -replace 'Demo', $name) | Set-Content -Path './CmakeLists.txt'
((Get-Content -path './InputManager.h' -Raw) -replace 'Demo', $name) | Set-Content -Path './InputManager.h'
((Get-Content -path "./$($name).cpp" -Raw) -replace 'Demo', $name) | Set-Content -Path "./$($name).cpp"
((Get-Content -path "./$($name).h" -Raw) -replace 'Demo', $name) | Set-Content -Path "./$($name).h"
((Get-Content -path './main.cpp' -Raw) -replace 'Demo', $name) | Set-Content -Path './main.cpp'
