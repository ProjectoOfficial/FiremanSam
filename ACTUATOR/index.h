#ifndef INDEX_H_
#define INDEX_H_

const char setup_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
  <html lang="en">
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>Fire Detector Configuration</title>
      
      <style>
        html {font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}

        body{ margin: 0;padding: 0;font-family: Arial, Helvetica, sans-serif;background-color: #a6e9d5;}

        .box{ width: 70%; padding: 40px; position: absolute; top: 50%; left: 50%; 
              transform: translate(-50%,-50%); background-color: #191919; color: white; 
              text-align: center; border-radius: 24px;}

        input{  border: 0; display: block; background: none; margin: 20px auto; text-align: center;
                border: 2px solid #0d6efd; padding: 14px 10px; width: 45%; outline: none; 
                border-radius: 24px; color: white; font-size: smaller; transition: 0.3s;}

        input:focus{ width: 90%; border-color:#6ea8fe ;}

        input[type='submit']{ border: 0; display: block; background: none; margin: 20px auto;
        text-align: center; border: 2px solid #0dcaf0; padding: 14px 10px; width: 140px; 
        outline: none; border-radius: 24px; color: white; transition: 0.3s; cursor: pointer;}
        
        input[type='submit']:hover{ background-color: #6edff6;}
      </style>
    
    </head>
  <body>
    <form action="/setup" class="box" id="my-form">
      <h1 class="display-1">Actuator Configuration</h1>
      <div class="part">
        <input name="ssid" type="text" placeholder="WiFi SSID">
      </div>
      <div class="part">
        <input name="password" type="password" placeholder="WiFi Password">
      </div>
      <div class="part">
        <input name="email" type="text" placeholder="Email Address">
      </div>
      <div class="part">
        <input name="device" type="text" placeholder="Device Name">
      </div>
      <input type="submit" value="Save">
    </form>
    
  </body>
</html>
)rawliteral";


const char fail_html1[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
  <html lang="en">
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>Fire Detector Configuration</title>
      
      <style>
        html {font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: left;}

        h1{text-align:center;}
        body{ margin: 0;padding: 0;font-family: Arial, Helvetica, sans-serif;background-color: #ea868f;}

        .box{ width: 70%; padding: 40px; position: absolute; top: 50%; left: 50%; 
              transform: translate(-50%,-50%); background-color: #191919; color: white; 
              text-align: left; border-radius: 24px;}

        input{  border: 0; display: block; background: none; margin: 20px auto; text-align: center;
                border: 2px solid #0d6efd; padding: 14px 10px; width: 45%; outline: none; 
                border-radius: 24px; color: white; font-size: smaller; transition: 0.3s;}

        input:focus{ width: 90%; border-color:#6ea8fe ;}

        
        .left{text-aling: left;}

        input[type='submit']{ border: 0; display: block; background: none; margin: 20px auto;
        text-align: center; border: 2px solid #0dcaf0; padding: 14px 10px; width: 140px; 
        outline: none; border-radius: 24px; color: white; transition: 0.3s; cursor: pointer;}
        
        input[type='submit']:hover{ background-color: #6edff6;}
      </style>
    
  </head>
  <body>
    <form action="/error" class="box" id="my-form">
      <h1 class="display-1">Error</h1>
      <ul class="left">
)rawliteral";


const char fail_html2[] PROGMEM = R"rawliteral(
</ul>
      <br />
      <input type="submit" value="Configure">
    </form>
  </body>
</html>
)rawliteral";

const char success_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
  <html lang="en">
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>Fire Detector Configuration</title>
      
      <style>
        html {font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}

        body{ margin: 0;padding: 0;font-family: Arial, Helvetica, sans-serif;background-color: #a6e9d5;}

        .box{ width: 70%; padding: 40px; position: absolute; top: 50%; left: 50%; 
              transform: translate(-50%,-50%); background-color: #191919; color: white; 
              text-align: center; border-radius: 24px;}

        input[type='submit']{ border: 0; display: block; background: none; margin: 20px auto;
        text-align: center; border: 2px solid #0dcaf0; padding: 14px 10px; width: 140px; 
        outline: none; border-radius: 24px; color: white; transition: 0.3s; cursor: pointer;}
        
        input[type='submit']:hover{ background-color: #6edff6;}
      </style>
    
  </head>
  <body>
    <form action="/success" class="box" id="my-form">
      <h1 class="display-1">Success</h1>
      <br />
      <input type="submit" value="Reboot">
    </form>
  </body>
</html>
)rawliteral";
  
#endif // INDEX_H_
