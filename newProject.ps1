$name = Read-Host "Enter dir name"
Copy-Item 'tutorial/Demo' -Destination "tutorial/$($name)"
Add-Content './tutorial/CmakeLists.txt' "`nadd_subdirectory($($name))"
CD tutorial/$name

Copy-Item '../Demo/Demo.cpp' -Destination "$($name).cpp"
Copy-Item '../Demo/BasicScene.h' -Destination "BasicScene.h"
Copy-Item '../Demo/BasicScene.cpp' -Destination "BasicScene.cpp"
Copy-Item '../Demo/CmakeLists.txt' -Destination "CmakeLists.txt"

((Get-Content -path './CmakeLists.txt' -Raw) -replace 'Demo', $name) | Set-Content -Path './CmakeLists.txt'
