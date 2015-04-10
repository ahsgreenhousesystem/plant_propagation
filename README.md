Current State:
 - Website builds in Arduino using the Wiznet Ethernet shield
 - Navigation between pages
 
TODO:
 - Use pinout from excel spreadsheet to define zones
 - Write config file after "Update zones" button pressed
 - Handle GET requests from html for changing zone states
 
HOW TO USE
 1) Load web interface files onto SD card
 2) Open webserver code in Arduino 
    - Modify IP address to suit network (obtain from school network administrator)
    - Upload to Arduino with ethernet shield installed
 3) Visit assigned IP address (~10 second load)
 4) Navigate to "Config > Setup Zones" to choose which zones to make visible and name specific zones
 5) Assign start times and durations to each zone
 6) Verify IO pinout with relays
