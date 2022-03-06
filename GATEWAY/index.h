#ifndef INDEX_H_
#define INDEX_H_

//**********************************CONFIGURATION SETUP**************************************
const char setup_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
  <html lang="en">
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>Gateway Configuration</title>
      
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
      <h1 class="display-1">Gateway Configuration</h1>
      <div class="part">
        <input name="ssid" type="text" placeholder="wifi ssid">
      </div>
      <div class="part">
        <input name="password" type="password" placeholder="wifi password">
      </div>
      <div class="part">
        <input name="email" placeholder="your contact email">
      </div>
      <div class="part">
        <input name="IPAddress" type="text" placeholder="static IP address">
      </div>
      <div class="part">
        <input name="IPRouter" type="text" placeholder="router IP address">
      </div>
      <div class="part">
        <input name="Subnet" type="text" placeholder="subnet mask">
      </div>
      <input type="submit" value="Save">
    </form>
    
  </body>
</html>
)rawliteral";


//**********************************CONFIGURATION FAIL**************************************
const char fail_html1[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
  <html lang="en">
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>Gateway Configuration</title>
      
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

//**********************************CONFIGURATION SUCCESS**************************************
const char success_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
  <html lang="en">
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>Gateway Configuration</title>
      
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


/*
 *                      GATEWAY WEB INTERFACE
 */

const char gateway_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
  <html lang="en">
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>Gateway Configuration</title>
      
      <style>
        html {font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}

        body{ margin: 0;padding: 0;font-family: Arial, Helvetica, sans-serif;background-color: #a6e9d5;}

        .box{ width: 70%; padding: 40px;  margin: auto; margin-top:25px;
              background-color: #191919; color: white; text-align: center; border-radius: 24px;}

        .unv {background-color: #191919; width: 73%; margin: auto; 
              padding: 16px; position: relative; min-width: 40px; list-style-type: none;}
         
        .nv{ display:inline; border: 0; background: none; margin: 20px auto; text-decoration: none;
        text-align: center; border: 2px solid #0dcaf0; padding: 14px 10px; width: 140px; 
        outline: none; border-radius: 5px; color: white; transition: 0.3s; cursor: pointer;}

        .nv:hover{background-color: #6edff6;}
        
        input{ border: 0; display: block; background: none; margin: 20px auto;
        text-align: center; border: 2px solid #0dcaf0; padding: 14px 10px; width: 140px; 
        outline: none; border-radius: 24px; color: white; transition: 0.3s; cursor: pointer;}
        
        input:hover{ background-color: #6edff6; color:#191919;}
      </style>
    
  </head>
  <body>
    <ul class="unv">
      <li><a class="nv" href="#reset">Reset</a></li>
    </ul>
  
    <div class="box">
      <h1 id="reset">Reset device</h1>
      <br/>
      <input type="button" value="Reset"/>
      <br/>
    </div>

    <div class="box">
      <h1 id="reset">Reset device</h1>
      <br/>
      <input type="button" value="Reset"/>
      <br/>
    </div>

  </body>
</html>
)rawliteral";


// NOME SENSORE - HUM - TEMP - CO2 - TVOC - NOME ALLARME -
// VANNO CONCATENATI NELLA SCRITTURA DELLA PAGINA WEB. 
const char BOX1[] PROGMEM = R"rawliteral(
<div class="box">
  <h2>Sensor: 
)rawliteral";  

const char BOX2[] PROGMEM = R"rawliteral(
  </h2>
  <ul>
    <li> Humidity: 
)rawliteral";

const char BOX3[] PROGMEM = R"rawliteral(
    % </li>
    <li> Temperature: 
)rawliteral";

const char BOX4[] PROGMEM = R"rawliteral(
    °C </li>
    <li> CO2: 
)rawliteral";

const char BOX5[] PROGMEM = R"rawliteral(
    ppm </li>
    <li> TVOC: 
)rawliteral";

//ALARM: on/off
const char BOX6[] PROGMEM = R"rawliteral(
    </li>
  </ul>
  <br />
  <h2>Alarm: 
)rawliteral";

const char BOX7[] PROGMEM = R"rawliteral(
    </h2>
</div>
)rawliteral";

#endif // INDEX_H_
