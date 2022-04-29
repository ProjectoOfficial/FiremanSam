#ifndef GATEWAY_H_
#define GATEWAY_H_

const char gateway_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
  <html lang="en">
    <head>
        <title>Gateway Configuration</title>
        <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/css/bootstrap.min.css" integrity="sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T" crossorigin="anonymous">
            
        <meta charset="utf-8">
        <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
  </head>

  <body>
  <h1 class="h1" style="text-align: center; margin: 0 auto;">Gateway home page</h1>
  <br />
    <div style="text-align: center; margin: 0 auto;">
      <h1 class="h3">Reset device</h1>
        <br/>
        <form action="/gateway">
          <button style="text-align: center; margin: 0 auto;" type="submit" id="reset" name="reset" class="btn btn-primary">Reset</button>
        </form>
        <br/>
    </div>
    <div style="text-align: center; margin: 0 auto;">
      <h1 class="h3">Pair</h1>
        <br/>
        <form action="/gateway">
          <button style="text-align: center; margin: 0 auto;" type="submit" id="pair" name="pair" class="btn btn-primary">Pair</button>
        </form>
    </div>
    
    <br />
    <br />

    <h1 class="h3" style="text-align: center; margin: 0 auto;">Pairs</h1>
)rawliteral";

const char gateway2_html[] PROGMEM = R"rawliteral(
  </body>
</html>
)rawliteral";


// SENSOR DISPLAY
const char sensor_html[] PROGMEM = R"rawliteral(
  <table style="text-align: center; margin: 0 auto; width: 65%;" class="table table-borderless bg-light">
    <tbody>
      <tr>
        <td class="align-middle">
          <span class="text-info align-middle"> Sensor: 
)rawliteral";


const char sensor2_html[] PROGMEM = R"rawliteral(
          </span>
        </td>

        <td class="align-middle">
          <span class="text-info align-middle"> Actuator:
)rawliteral";


const char sensor3_html[] PROGMEM = R"rawliteral(
        </td>
      </tr>
)rawliteral";


const char sensornomore_html[] PROGMEM = R"rawliteral(
    </tbody>
  </table>
  <br />
)rawliteral";

const char sensor4_html[] PROGMEM = R"rawliteral(

      <tr>
        <td class="align-middle">
          <span class="text-info align-middle"></span>
        </td>
        <td class="align-middle">
          <span class="text-info align-middle"> Actuator:
)rawliteral";

const char sensor5_html[] PROGMEM = R"rawliteral(
          </span>
        </td>
      </tr>
    </tbody>
  </table>
  <br />
)rawliteral";

// PAIRING
const char pair_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
    <head>
        <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/css/bootstrap.min.css" integrity="sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T" crossorigin="anonymous">
        
        <meta charset="utf-8">
        <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    
    </head>
    <body>
        <div style="text-align: center; margin: 0 auto;">
          <h1>Home</h1>
            <br/>
            <form action="/pair">
              <button style="text-align: center; margin: 0 auto;" type="submit" id="home" name="home" class="btn btn-primary">Home</button>
            </form>
            <br/>
        </div>
    
        <form class="row gy-2 gx-3 align-items-center" action="/pair" style="text-align: center; margin: 0 auto; width: 50%;">
            <div class="col-md-6" >
                <label class="visually-hidden" for="Sensori">Sensori</label>
                <br />
                <select class="form-select" name="sensori" id="sensori">
)rawliteral";


const char pair_html2[] PROGMEM = R"rawliteral(
                </select>
            </div>

            <div class="col-md-6">
                <label class="visually-hidden" for="Attuatori">Attuatori</label>
                <br />
                <select class="form-select" name="attuatori" id="attuatori">
)rawliteral";


const char pair_html3[] PROGMEM = R"rawliteral(
                </select>
            </div>
            <br />
            <button style="text-align: center; margin: 0 auto;" id="pair" name="pair" type="submit" class="btn btn-primary">Pair</button>
            <button style="text-align: center; margin: 0 auto;" id="unpair" name="unpair" type="submit" class="btn btn-primary">Unpair</button>
        </form>

        <script src="https://code.jquery.com/jquery-3.3.1.slim.min.js" integrity="sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo" crossorigin="anonymous"></script>
        <script src="https://cdn.jsdelivr.net/npm/popper.js@1.14.7/dist/umd/popper.min.js" integrity="sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1" crossorigin="anonymous"></script>
        <script src="https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/js/bootstrap.min.js" integrity="sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM" crossorigin="anonymous"></script>
    </body>
</html>
)rawliteral";

#endif // GATEWAY_H_
