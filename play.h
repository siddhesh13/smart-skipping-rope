const char MAIN_page[] PROGMEM = R"=====(
  <!DOCTYPE html>
  <html>
    <head>
      <meta charset="utf-8">
      <title>Smart Skipping Rope</title>
      <style media="screen">
        html {
          background-color:#89D0BE;
        }
                table, th, td {
    //border: 1px solid yellow;
    border-collapse: collapse;
}
th{
    padding: 5px;
    text-align: left;
    font-size: 1.2em;
    color:white;
}
td {
    padding: 10px;
    text-align: left;
    font-size: 1.7em;
    color:white;
}
table#t01 {
    width: 100%;    
    background-color: #89D0BE;
}
        #score {
          text-align: center;
          font-size: 20em;
          margin:90px 0px 10px 0px;
          font-family: sans-serif;
          color:white;
          text-shadow:5px 5px #00A1A3;
        }
        #sr {
          padding: 10px;
          text-align: left;
          font-size: 1.9em;
          color:white;
          margin:90px 0px 10px 0px;
          font-family: sans-serif;
          color:white;
        }
        #calory {
          padding: 10px;
          text-align: left;
          font-size: 1.9em;
          color:white;
          margin:90px 0px 10px 0px;
          font-family: sans-serif;
          color:white;
        }
        #score.game-over {
          font-size:2em;
        }
        #message {
          display:none;
          text-align: center;
          font-size: 2em;
          font-family: sans-serif;
          color:white;
          margin-top:5px;
          text-shadow:5px 5px #00A1A3;
          text-decoration: blink;
        }
        #message.active, #score.game-over {
          display:block;
          animation: blink-animation 1.5s steps(5, start) infinite;
          -webkit-animation: blink-animation 1.5s steps(5, start) infinite;
        }
        @keyframes blink-animation {
          to {
            visibility: hidden;
          }
        }
        @-webkit-keyframes blink-animation {
          to {
            visibility: hidden;
          }
        }
      </style>
    </head>
    <body>
     <table id="t01">
  <tr><th>Skipping Rate</th> 
    <th>Calories burnt</th>
    <th>Weight(kg):<input class="enabled" id="r" type="range" min="0" max="120" step="1" oninput="sendWght();" value="50"></th>
    </tr>
  <tr>
    <td id="sr">0 spm</td>
    <td id="calory">0</td>
    <td id="weight">Weight:</td>
  </tr>

 
</table>
      <h1 id="score">0</h1>
      <h4 id="message"></h4>
     
      <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js"></script>
      <script type="text/javascript">
      var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);
         var c = "";
        var s="";
      connection.onmessage = function (e) {

        console.log(e.data);
       // var count = JSON.parse(e.data);
        //document.getElementById("sr").innerHTML = count.srate + " spm";
       // document.getElementById("score").innerHTML = count.score;
     
        if (e.data.charAt(0) === "T") {
           //$("#score").addClass("game-over");
           $("#message").addClass("active");
           document.getElementById("message").innerHTML = e.data;
          
        }
        else if (e.data.charAt(0) === "S") {
  
           document.getElementById("sr").innerHTML = e.data.substring(1) + " spm";

        }
        else if (e.data.charAt(0) === "$") {
           document.getElementById("calory").innerHTML = e.data.substring(1) + " Kcal";
          
        }else {

           document.getElementById("score").innerHTML = e.data;
         }
      };
      
      var rainbowEnable = false;
      function sendWght () {
  var w = document.getElementById('r').value;
  var wStr = '#' + r.toString();
  document.getElementById("weight").innerHTML = "Weight: " + w+" Kg";
  console.log('RGB: ' + wStr);
  connection.send(wStr);
}

      </script>
    </body>
  </html>


)=====";
