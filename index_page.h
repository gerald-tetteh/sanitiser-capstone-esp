const char index_html[] PROGMEM = R"rawliteral(
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
      }

      .heading {
        font-weight: 600;
        text-align: center;
      }

      .form {
        max-width: 600px;
        display: flex;
        flex-direction: column;
        margin: 2rem auto;
      }

      .form-group {
        display: flex;
        flex-direction: column;
        margin-bottom: 1rem;
      }

      .form-group label {
        font-size: 2rem;
      }

      .form-input {
        padding: 1rem 1.2rem;
        font-size: 1.6rem;
        font-family: 'Open Sans', sans-serif;
        margin-top: 0.5rem;
        border-radius: 3px;
        outline: none;
        border: 3px solid transparent;
      }

      .form-input::placeholder {
        font-family: 'Open Sans', sans-serif;
        font-size: 1.6rem;
      }

      .form-input:focus {
        border: 3px solid teal;
      }

      input[type="submit"] {
        border-radius: 3px;
        margin-top: 1rem;
        border: none;
        padding: 1rem 1.5rem;
        font-size: 1.6rem;
        background-color: teal;
        color: white;
        text-transform: uppercase;
      }

      @media only screen and (max-width: 56.25em) {
        body {
          padding: 4rem;
        }
      }
    </style>
  </head>

  <body>
    <h1 class="heading">AHSM WIFI & Notification Configuration</h1>
    <form class="form" action="/connect" method="post">
      <div class="form-group">
        <label for="email">Email</label>
        <input class="form-input" type="email" name="email" placeholder="Email for Notification" id="email">
      </div>
      <div class="form-group">
        <label for="ssid">SSID</label>
        <input class="form-input" type="text" name="ssid" placeholder="AddoNet" id="ssid">
      </div>
      <div class="form-group">
        <label for="password">Password</label>
        <input class="form-input" type="password" name="password" placeholder="Password" id="password">
      </div>
      <input type="submit" value="submit">
    </form>
  </body>

</html>
)rawliteral";