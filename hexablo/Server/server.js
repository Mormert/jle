const port = process.env.PORT || 314;

const express = require('express');
const session = require("express-session");

const bodyParser = require("body-parser");
const passport = require("passport");

const app = express();

const sessionMiddleware = session({ secret: "super-duper-mega-secret", resave: false, saveUninitialized: false });
app.use(sessionMiddleware);
app.use(express.json());
app.use(bodyParser.urlencoded({ extended: false }));
app.use(passport.initialize());
app.use(passport.session());
app.use(express.static(__dirname + '/public'));
app.set('view engine', 'ejs');

const httpServer = require("http").createServer(app);

const io = require("socket.io")(httpServer, {
    cors: { origin: "http://localhost/", methods: ["GET", "POST"], credentials: true },
   // parser: require("socket.io-msgpack-parser")
});

g_connected_players_counter = 0;

io.on('connection', client => {

    console.log(client.id + " connected.");

    let message = { 'e': "player_connect", 'j': { 'id': client.id } };
    client.broadcast.emit("msgpack", JSON.stringify(message));

    // If no connected players, give host to first connection
    if (g_connected_players_counter == 0) {
        let message = { 'e': "you_r_host", 'j': {} };

        client.emit("msgpack", JSON.stringify(message));
        console.log("Host assignment done.");
    }

    g_connected_players_counter++;

    client.on('msgpack', data => {

        try{
            // We need to parse JSON string
            data = JSON.parse(data);
            console.log("recv data: " + JSON.stringify(data));
        } catch {
            // But if send from browser, it is already a JSON object
            console.log("recv data: " + data);
        }

        data.s = client.id; // add the sender to data
        if (data.r == "") {
            // Sent to all players except sender
            client.broadcast.emit("msgpack", JSON.stringify(data));
            console.log("Sent to all players except sender");
        } else if (data.r == "HOST") {
            // Send only to the game host
            io.to(g_host_socket_id).emit("msgpack", JSON.stringify(data));
            console.log("Sent to the game host");
        } else {
            // Send to the given socket id
            io.to(data.r).emit("msgpack", JSON.stringify(data));
            console.log("Sent to the given socket id");
        }
    });

    client.on('disconnect', () => {
        console.log(client.id + " disconnected.");

        // Sent to all players except sender
        let message = { 'e': "player_dconnect", 'j': { 'id': client.id } };
        client.broadcast.emit("msgpack", JSON.stringify(message));

        g_connected_players_counter--;
    });

});


httpServer.listen(port);
console.log("Server started on port: " + port);

