| CURRENT STATE |
| ----- |
| Website builds in Arduino using the Wiznet Ethernet shield |
| Navigation between pages |
| Ability to turn sprinklers on/off from web interface |

| HOW TO USE |
| ----- |
| 1) Load web interface files onto SD card |
| 2) Open webserver code in Arduino |
| - Modify IP address to suit network (obtain from school network administrator) |
| - Upload to Arduino with ethernet shield installed |
| 3) Visit assigned IP address (about 5 second load) |
| 4) Navigate to "Config > Setup Zones" to choose which zones to make visible and name specific zones |
| 5) Assign start times and durations to each zone |
| 6) Verify IO pinout with relays |

| HOW TO USE - UDOO Server |
| ----- |
| Navigate to folder with core.coffee using Terminal |
| Type "coffee core.coffee" to start the server |
| Go to homepage in your browser on port 3000 at http://localhost:3000/index.html |