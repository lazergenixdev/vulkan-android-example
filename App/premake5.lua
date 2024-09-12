
project 'App'

kind 'WindowedApp'

targetdir '%{wks.location}/bin/%{cfg.system}-%{cfg.buildcfg}'

files { 'src/**' }

engineinclude ()

links { 'Engine' }

android {
    name = 'App',
}