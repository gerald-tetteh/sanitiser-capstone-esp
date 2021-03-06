const char complete_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">

  <head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Config AHSM WIFI Connection</title>
    <style>
      @import url('https://fonts.googleapis.com/css2?family=Open+Sans:ital,wght@0,300;0,400;0,500;0,600;0,700;0,800;1,300;1,400;1,500;1,600;1,700;1,800&display=swap');

      *,
      *::before,
      *::after {
        box-sizing: border-box;
        margin: 0;
        padding: 0;
      }

      html {
        font-size: 62.5%;
      }

      body {
        font-family: 'Open Sans', sans-serif;
        font-size: 1.6rem;
        padding: 10rem;
        background-color: #212121;
        color: white;
        text-align: center;
      }

      .heading {
        margin-bottom: 1rem;
      }

      .link,
      .link:link,
      .link:visited {
        text-decoration: none;
        color: teal;
        font-size: 1.8rem;
      }

      @media only screen and (max-width: 56.25em) {
        body {
          padding: 4rem;
        }
      }
    </style>
  </head>

  <body>

    <h1 class="heading">Configuration Complete</h1>
    <p class="content">
      Press the Operation button machine to continue use.
    </p>
    <p class="content">
      If you want to reconfigure click <a class="link" href="/">here</a>
    </p>

  </body>

</html>
)rawliteral";
