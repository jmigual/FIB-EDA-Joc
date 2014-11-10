// *********************************************************************
// Global variables
// *********************************************************************

var PI = 3.14159265358979323846

var randomNumber = 1;

// Viewer state
var gamePaused = true;
var gamePreview = false; //If true, render will be called for the next tick even if game is paused, and then will be set to false
var gameDirection = 1;
var gameAnim = true;
var actRound = 0; //Current round index

// Data
var raw_data_str; //String for storing the raw data
var dataLoaded = false; //Set to true when raw_data_str is ready to be parsed
var data = { } //Object for storing all the game data

// Animation
var speed = 100; //Ticks per second
var FRAMES_PER_ROUND = 4;
var frames = 0; //Incremented each tick, when it reaches FRAMES_PER_ROUND, actRound is updated (acording to gameDirection)

// Visuals
var unitSize = 0.6; // 1 = same size as tile
var unitLineWidth = 2;
var grid_color = "#888888";
var player_colors = {
    '0': "#000085",
    '1': "#900090",
    '2': "#FF0000",
    '3': "#FFD700",
}

var SOLDAT = 1;
var HELI   = 2;

var VALOR_ALT  = 4000;
var VALOR_BAIX = 1000;


var rot = {
    '0': -PI/2,
    '1': PI,
    '2': PI/2,
    '3': 0,
}


// *********************************************************************
// Utility functions
// *********************************************************************


function getURLParameter (name) {
    // http://stackoverflow.com/questions/1403888/get-url-parameter-with-jquery
    var a = (RegExp(name + '=' + '(.+?)(&|$)').exec(location.search)||[,null])[1]
    if (a != null) return decodeURI(a);
    return null;
}

//Callback has a single parameter with the file contents
function loadFile (file, callback) {

    var xmlhttp;

    if (file == null || file == "") {
        alert("You must specify a file to load");
        return;
    }

    if (window.XMLHttpRequest) {
        // code for IE7+, Firefox, Chrome, Opera, Safari
        xmlhttp = new XMLHttpRequest();
    } else {
        // code for IE6, IE5
        xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
    }

    // http://www.w3schools.com/ajax/ajax_xmlhttprequest_onreadystatechange.asp
    xmlhttp.onreadystatechange = function() {
        if (xmlhttp.readyState == 4) {
            //Note: We can not check xmlhttp.status != 200 for errors because status is not set when loading local files
            callback(xmlhttp.responseText);
        }
    }

    xmlhttp.open("GET", file, false);
    xmlhttp.send();
}

function int (s) {
    return parseInt(s);
}

function double (s) {
    return parseFloat(s);
}


function parse_assert(read_value, expected_value) {
    var correct = (read_value == expected_value);
    if (!correct) alert("Error parsing file, expected token: " + expected_value + ", read token: " + read_value);
    return correct;
}

// *********************************************************************
// Initialization functions
// *********************************************************************

function parseData (raw_data_str) {

    data.nb_rounds  = 200;
    data.nb_players =   4;
    data.nb_rows    =  60;
    data.nb_cols    =  60;
    data.nb_posts   =  16;

    data.tileSize = 50;

    if ("" == raw_data_str) {
        alert("Could not load game file");
        return false;
    }

    // convert text to tokens
    var st = raw_data_str + "";
    var t = st.replace('\n', ' ').split(/\s+/);
    var p = 0;

    data.secgame = (t[p++] == "SecGame");

    //game and version
    if (t[p++] != "ApocalypseNow") {
	alert("Are you sure this is an Apocalypse Now game file?");
	document.getElementById('file').value = "";
	document.getElementById('inputdiv').style.display = "";
	document.getElementById('loadingdiv').style.display = "none";
	return false;
    }
    data.version = t[p++];
    if (data.version != "v2") {
        alert("Unsupported game version! Trying to load it anyway.");
    }

    parse_assert(t[p++], "names");
    data.names = new Array();
    for (var i = 0; i < data.nb_players; ++i) {
        data.names[i] = t[p++];
    }

    data.rounds = new Array();
    for (var round = 0; round <= data.nb_rounds; ++round) {

        parse_assert(t[p++], "round");
        if (int(t[p++]) != round) alert("Wrong round number!");

        data.rounds[round] = new Object();

	// nou: skip
        parse_assert(t[p++], "nou");
	data.rounds[round].nou = int(t[p++]);

        // maze
        parse_assert(t[p++], "terreny");
	p++; // parse_assert(t[p++], "00000000001111111111222222222233333333334444444444555555555566666666667777777777");
        p++; // parse_assert(t[p++], "01234567890123456789012345678901234567890123456789012345678901234567890123456789");

        data.rounds[round].rows = new Array();
        for (var i = 0; i < data.nb_rows; ++i) {
            var aux = t[p++];
	    var correct = (int(aux) == i);
	    if (!correct) alert("Error parsing file, read token: " + aux);
            data.rounds[round].rows[i] = t[p++];
        }

	// posts
        parse_assert(t[p++], "posts");
        parse_assert(t[p++], "equip");
        parse_assert(t[p++], "x");
        parse_assert(t[p++], "y");
        parse_assert(t[p++], "valor");
	data.rounds[round].posts = new Array();
	for (var pt = 0; pt < data.nb_posts; ++pt) {
	    data.rounds[round].posts[pt] = new Object();
	    data.rounds[round].posts[pt].equip = int(t[p++]);
	    data.rounds[round].posts[pt].x     = int(t[p++]);
	    data.rounds[round].posts[pt].y     = int(t[p++]);
	    data.rounds[round].posts[pt].valor = int(t[p++]);
	}

        // score
        parse_assert(t[p++], "score");
        data.rounds[round].score = new Array();
        for (var i = 0; i < data.nb_players; ++i) {
            data.rounds[round].score[i] = int(t[p++]);
        }

        // status
        parse_assert(t[p++], "status");
        data.rounds[round].cpu = new Array();
        for (var i = 0; i < data.nb_players; ++i) {
            var cpu = int(double(t[p++])*100);
            data.rounds[round].cpu[i] = (cpu == -100)? "out" : cpu+"%";
        }

        //agents
        parse_assert(t[p++], "agents");
        parse_assert(t[p++], "id");
        parse_assert(t[p++], "tipus");
        parse_assert(t[p++], "equip");
        parse_assert(t[p++], "x");
        parse_assert(t[p++], "y");
        parse_assert(t[p++], "vida");
        parse_assert(t[p++], "ori");
        parse_assert(t[p++], "napalm");
        parse_assert(t[p++], "paraca");

	data.rounds[round].id2pos = new Array();
	for (var i = 0; i < data.rounds[round].nou; ++i) {
	    data.rounds[round].id2pos[i] = -1;
	}

        data.rounds[round].agents = new Array();
	var id = int(t[p++]);
	var i = 0;
        while (id != -1) {
	    data.rounds[round].agents[i] = new Object();
	    data.rounds[round].agents[i].id     = id;
	    data.rounds[round].id2pos[id] = i;
	    data.rounds[round].agents[i].tipus  = int(t[p++]);
	    data.rounds[round].agents[i].equip  = int(t[p++]);
	    data.rounds[round].agents[i].x      = int(t[p++]);
	    data.rounds[round].agents[i].y      = int(t[p++]);
	    data.rounds[round].agents[i].vida   = int(t[p++]);
	    data.rounds[round].agents[i].ori    = int(t[p++]);
	    data.rounds[round].agents[i].napalm = int(t[p++]);
	    var sz = int(t[p++]);
	    for (var j = 0; j < sz; ++j) p++;
	    id = int(t[p++]);
            ++i;
	}
	data.rounds[round].agents_size = i;

	// foc
        parse_assert(t[p++], "foc");
        parse_assert(t[p++], "torns");
        parse_assert(t[p++], "x");
        parse_assert(t[p++], "y");
	data.rounds[round].foc = new Array();
	var torns = int(t[p++]);
	var i = 0;
	while (torns != -1) {
	    data.rounds[round].foc[i] = new Object();
	    data.rounds[round].foc[i].torns = torns;
	    data.rounds[round].foc[i].x     = int(t[p++]);
	    data.rounds[round].foc[i].y     = int(t[p++]);
	    torns = int(t[p++]);
	    ++i;
	}
	data.rounds[round].foc_size = i;

        if (round != data.nb_rounds) {
            // actions asked: skip
            parse_assert(t[p++], "actions_asked");
            for (var pl = 0; pl < data.nb_players; ++pl) {
                if (int(t[p++]) != pl+1) alert("Wrong player id!");

		// soldats
                var code = int(t[p++]);
                while (code != -1) {
		    p+=2;
                    code = int(t[p++]);
                }

		// helicopters
                code = int(t[p++]);
                while (code != -1) {
		    p++;
                    code = int(t[p++]);
                }

		// paracas
                code = int(t[p++]);
                while (code != -1) {
		    p++;
                    code = int(t[p++]);
                }
            }

            // actions_done
            parse_assert(t[p++], "actions_done");
	    data.rounds[round].player = new Array();
            for (var pl = 0; pl < data.nb_players; ++pl) {
                if (int(t[p++]) != pl+1) alert("Wrong player id!");

		data.rounds[round].player[pl] = new Object();
		var i, id;

		data.rounds[round].player[pl].actions_solds = new Array();
		id = int(t[p++]);
		i = 0;
		while (id != -1) {
		    data.rounds[round].player[pl].actions_solds[i] = new Object();
                    data.rounds[round].player[pl].actions_solds[i].id = id;
                    data.rounds[round].player[pl].actions_solds[i].x  = int(t[p++]);
                    data.rounds[round].player[pl].actions_solds[i].y  = int(t[p++]);
                    id = int(t[p++]);
		    ++i;
		}
		data.rounds[round].player[pl].actions_solds_size = i;


		data.rounds[round].player[pl].actions_helis = new Array();
		id = int(t[p++]);
		i = 0;
		while (id != -1) {
		    data.rounds[round].player[pl].actions_helis[i] = new Object();
                    data.rounds[round].player[pl].actions_helis[i].id  = id;
                    data.rounds[round].player[pl].actions_helis[i].act = int(t[p++]);
                    id = int(t[p++]);
		    ++i;
		}
		data.rounds[round].player[pl].actions_helis_size = i;

		data.rounds[round].player[pl].actions_paras = new Array();
		var x = int(t[p++]);
		i = 0;
		while (x != -1) {
		    data.rounds[round].player[pl].actions_paras[i] = new Object();
                    data.rounds[round].player[pl].actions_paras[i].x  = x;
                    data.rounds[round].player[pl].actions_paras[i].y = int(t[p++]);
                    x = int(t[p++]);
		    ++i;
		}
		data.rounds[round].player[pl].actions_paras_size = i;
            }
        }
    }
    return true;
}


//Initializing the game
function initGame (raw_data) {

    document.getElementById("loadingdiv").style.display="";

    //TODO: Next two calls could run concurrently
    if (parseData(raw_data) === false) return;
    preloadImages();

    // prepare state variables
    /*if (getURLParameter("start") == "yes") gamePaused = false;
      else gamePaused = true;*/
    gamePaused = false;

    gamePreview = true;

    // slider init
    $("#slider").slider({
	min: 0,
	max: data.nb_rounds,
    });

    // Canvas element
    canvas = document.getElementById('myCanvas');
    context = canvas.getContext("2d");

    // prepare the slider
    $("#slider").slider({
        slide: function(event, ui) {
            var value = $("#slider").slider( "option", "value" );
            actRound = value;
            frames = 0;
            gamePaused = true;
            gamePreview = true;
        }
    });
    $("#slider").width(500);

    // set the listerners for interaction
    document.addEventListener('mousewheel', onDocumentMouseWheel, false);
    document.addEventListener('keydown', onDocumentKeyDown, false);
    document.addEventListener('keyup', onDocumentKeyUp, false);

    window.addEventListener('resize', onWindowResize, false);
    onWindowResize();

    document.getElementById("loadingdiv").style.display="none";
    document.getElementById("gamediv").style.display="";

    mainloop();
}


function preloadImages () {

    data.img = new Array();

    // Grass
    data.img.grass = new Array();

    data.img.grass[0] = new Image();
    data.img.grass[0].src = "img/grass0.png";

    data.img.grass[1] = new Image();
    data.img.grass[1].src = "img/grass1.png";

    data.img.grass[2] = new Image();
    data.img.grass[2].src = "img/grass2.png";

    data.img.grass[3] = new Image();
    data.img.grass[3].src = "img/grass3.png";

    data.img.grass[4] = new Image();
    data.img.grass[4].src = "img/grass4.png";


    // Tree
    data.img.tree = new Image();
    data.img.tree.src = "img/tree.png";

    // Water
    data.img.water = new Image();
    data.img.water.src = "img/water.png";


    // Mountain
    data.img.mountain = new Image();
    data.img.mountain.src = "img/mountain.png";


    // Posts
    data.img.hi_posts = new Array();

    data.img.hi_posts[0] = new Array();
    data.img.hi_posts[0][0] = new Image();
    data.img.hi_posts[0][0].src = "img/square0.000085.png";
    data.img.hi_posts[0][1] = new Image();
    data.img.hi_posts[0][1].src = "img/square1.000085.png";

    data.img.hi_posts[1] = new Array();
    data.img.hi_posts[1][0] = new Image();
    data.img.hi_posts[1][0].src = "img/square0.900090.png";
    data.img.hi_posts[1][1] = new Image();
    data.img.hi_posts[1][1].src = "img/square1.900090.png";

    data.img.hi_posts[2] = new Array();
    data.img.hi_posts[2][0] = new Image();
    data.img.hi_posts[2][0].src = "img/square0.FF0000.png";
    data.img.hi_posts[2][1] = new Image();
    data.img.hi_posts[2][1].src = "img/square1.FF0000.png";

    data.img.hi_posts[3] = new Array();
    data.img.hi_posts[3][0] = new Image();
    data.img.hi_posts[3][0].src = "img/square0.FFD700.png";
    data.img.hi_posts[3][1] = new Image();
    data.img.hi_posts[3][1].src = "img/square1.FFD700.png";

    data.img.hi_posts[4] = new Array();
    data.img.hi_posts[4][0] = new Image();
    data.img.hi_posts[4][0].src = "img/square0.FFFFFF.png";
    data.img.hi_posts[4][1] = new Image();
    data.img.hi_posts[4][1].src = "img/square1.FFFFFF.png";

    data.img.lo_posts = new Array();

    data.img.lo_posts[0] = new Image();
    data.img.lo_posts[0].src = "img/square.000085.png";

    data.img.lo_posts[1] = new Image();
    data.img.lo_posts[1].src = "img/square.900090.png";

    data.img.lo_posts[2] = new Image();
    data.img.lo_posts[2].src = "img/square.FF0000.png";

    data.img.lo_posts[3] = new Image();
    data.img.lo_posts[3].src = "img/square.FFD700.png";

    data.img.lo_posts[4] = new Image();
    data.img.lo_posts[4].src = "img/square.FFFFFF.png";


    // Fire
    data.img.fire = new Array();

    data.img.fire[0] = new Image();
    data.img.fire[0].src = "img/fire0.png";

    data.img.fire[1] = new Image();
    data.img.fire[1].src = "img/fire1.png";


    // Soldiers
    data.img.soldier = new Array();

    data.img.soldier[0] = new Image();
    data.img.soldier[0].src = "img/dot.000085.png";

    data.img.soldier[1] = new Image();
    data.img.soldier[1].src = "img/dot.900090.png";

    data.img.soldier[2] = new Image();
    data.img.soldier[2].src = "img/dot.FF0000.png";

    data.img.soldier[3] = new Image();
    data.img.soldier[3].src = "img/dot.FFD700.png";


    // Helicopters
    data.img.heli = new Array();

    data.img.heli[0] = new Array();

    data.img.heli[0][0] = new Image();
    data.img.heli[0][0].src = "img/helicopter.x.000085.png";

    data.img.heli[0][1] = new Image();
    data.img.heli[0][1].src = "img/helicopter.x.900090.png";

    data.img.heli[0][2] = new Image();
    data.img.heli[0][2].src = "img/helicopter.x.FF0000.png";

    data.img.heli[0][3] = new Image();
    data.img.heli[0][3].src = "img/helicopter.x.FFD700.png";

    data.img.heli[1] = new Array();

    data.img.heli[1][0] = new Image();
    data.img.heli[1][0].src = "img/helicopter.+.000085.png";

    data.img.heli[1][1] = new Image();
    data.img.heli[1][1].src = "img/helicopter.+.900090.png";

    data.img.heli[1][2] = new Image();
    data.img.heli[1][2].src = "img/helicopter.+.FF0000.png";

    data.img.heli[1][3] = new Image();
    data.img.heli[1][3].src = "img/helicopter.+.FFD700.png";
}




// *********************************************************************
// Main loop functions
// *********************************************************************

function updateGame () {
    /*
      if (actRound >= 0 && actRound < data.nb_rounds) {
      for (var i = 0; i < data.nb_players; ++i) {
      var f = (frames+i)%4;
      switch (data.rounds[actRound].team[i].pacman.action) {
      case 't': //Top
      data.img.spr_pacman[i] = data.img.spr_pacman_t[i][f];
      break;
      case 'b': //Bottom
      data.img.spr_pacman[i] = data.img.spr_pacman_b[i][f];
      break;
      case 'r': //Right
      data.img.spr_pacman[i] = data.img.spr_pacman_r[i][f];
      break;
      case 'l': //Left
      data.img.spr_pacman[i] = data.img.spr_pacman_l[i][f];
      break;
      default: //None
      data.img.spr_pacman[i] = data.img.spr_pacman_r[i][f];
      break;
      }   }   }
    */
    $("#slider").slider("option", "value", actRound);
}


function writeGameState () {
    // write round
    $("#round").html("Round: " + actRound);

    //update scoreboard
    var scoreboard = "";
    for (var i = 0; i <= 2; ++i)
	scoreboard += "<br/><br/>";
    for (var i = 0; i < data.nb_players; i++) {
        scoreboard += "<span class='score'>"
            + "<div style='display:inline-block; margin-top: 5px; width:20px; height:20px; background-color:"+ player_colors[i] +"'></div>"
            + "<div style='display:inline-block; vertical-align: middle; margin-bottom: 7px; margin-left:8px;'>"+data.names[i]+"</div>"
            + "<br/>"
            + "<div style='margin-left: 10px;'>"
            + "<div style='padding:2px;'>Score: "+data.rounds[actRound].score[i]+"</div>"
            + (data.secgame? "<div style='padding:2px;'>CPU: " + data.rounds[actRound].cpu[i] + "</div>" : "")
            + "</div>"
            + "</span><br/><br/><br/>";
    }
    $("#scores").html(scoreboard);
}

function drawGame () {

    randomNumber = 1;

    //Boundary check
    if (actRound < 0) actRound = 0;
    if (actRound >= data.nb_rounds) actRound = data.nb_rounds;

    if (canvas.getContext) {
        var context = canvas.getContext('2d');
        var rectSize = data.tileSize;

	var fixedMargin = 0; // 10;
	var heightScore = 120; // 180;

	canvas.width  = window.innerWidth  - 2*fixedMargin;
        canvas.height = window.innerHeight - 2*fixedMargin - heightScore;

	var sw = canvas.width  / (rectSize*data.nb_cols);
	var sh = canvas.height / (rectSize*data.nb_rows);
	var s;
	if (sw < sh) {
	    s = sw;
	    var offset = (canvas.height - s*rectSize*data.nb_rows)/ 2;
	    canvas.style.marginTop  = fixedMargin + offset;
	    canvas.style.marginLeft = fixedMargin;
	}
	else {
	    s = sh;
	    var offset = (canvas.width - s*rectSize*data.nb_cols)/ 2;
	    canvas.style.marginTop  = fixedMargin;
	    canvas.style.marginLeft = fixedMargin + offset;
	}
        context.scale(s, s);

        // outer rectangle
        context.fillStyle = "rgb(30,30,30)";
        context.fillRect(0, 0, rectSize*data.nb_cols, rectSize*data.nb_rows);

	// Draw background
	for (var i = 0; i < data.nb_rows; i++)
            drawBackground(actRound, i);

	// Draw posts
	for (var pt = 0; pt < data.nb_posts; ++pt) {
	    var     x = data.rounds[actRound].posts[pt].x;
	    var     y = data.rounds[actRound].posts[pt].y;
	    var equip = data.rounds[actRound].posts[pt].equip;
	    var valor = data.rounds[actRound].posts[pt].valor;
	    if (equip == -1) equip = 4;
	    else --equip;
	    var rectSize = data.tileSize;
	    var post;
	    if (valor == VALOR_ALT) {
		var param;
		if (actRound % 4 < 2) param = 0;
		else                  param = 1;
		if (gameAnim) post = data.img.hi_posts[equip][param];
		else          post = data.img.hi_posts[equip][0];
	    }
	    else {
		post = data.img.lo_posts[equip];
	    } 
	    context.drawImage(post, (y-1)*rectSize, (x-1)*rectSize);
	}

	// Draw forest
	for (var i = 0; i < data.nb_rows; i++)
            drawForest(actRound, i);

	// Draw fire
	for (var k = 0; k < data.rounds[actRound].foc_size; ++k) {
	    if (data.rounds[actRound].foc[k].torns > 0) {
		var x = data.rounds[actRound].foc[k].x;
		var y = data.rounds[actRound].foc[k].y;
		var fire;
		if (gameAnim) fire = data.img.fire[frames % 2]; 
		else          fire = data.img.fire[0]; 
		context.drawImage(fire, y*data.tileSize, x*data.tileSize);
	    }
	}

	// Draw soldiers.
	for (var i = 0; i < data.rounds[actRound].agents_size; ++i) {
            var id    = data.rounds[actRound].agents[i].id;
            var x     = data.rounds[actRound].agents[i].x;
            var y     = data.rounds[actRound].agents[i].y;
	    var equip = data.rounds[actRound].agents[i].equip;
	    --equip;
	    var tipus = data.rounds[actRound].agents[i].tipus;
	    if (tipus == SOLDAT) {
		if (actRound < data.nb_rounds && gameAnim) {
		    var pos  = data.rounds[actRound+1].id2pos[id];
		    if (pos != -1) {
			var nx   = data.rounds[actRound+1].agents[pos].x;
			var ny   = data.rounds[actRound+1].agents[pos].y;
			x = correctX(x, nx);
			y = correctY(y, ny);
		    }
		}
		var soldier = data.img.soldier[equip];
		context.drawImage(soldier, y*data.tileSize, x*data.tileSize);
	    }
	}

	// Draw helicopters.
	for (var i = 0; i < data.rounds[actRound].agents_size; ++i) {
            var id    = data.rounds[actRound].agents[i].id;
            var x     = data.rounds[actRound].agents[i].x;
            var y     = data.rounds[actRound].agents[i].y;
	    var equip = data.rounds[actRound].agents[i].equip;
	    --equip;
	    var tipus = data.rounds[actRound].agents[i].tipus;
	    if (tipus != SOLDAT) {
		var ori = data.rounds[actRound].agents[i].ori;
		var diff = 0;
		if (actRound < data.nb_rounds && gameAnim) {
		    var pos  = data.rounds[actRound+1].id2pos[id];
		    var nx   = data.rounds[actRound+1].agents[pos].x;
		    var ny   = data.rounds[actRound+1].agents[pos].y;
		    var nori = data.rounds[actRound+1].agents[pos].ori;
		    var diff = nori - ori;
		    if (diff ==  3) diff = -1;
		    if (diff == -3) diff =  1;
		    x = correctX(x, nx);
		    y = correctY(y, ny);
		}
		var heli;
		if (gameAnim) {
		    heli = data.img.heli[frames % 2][equip];
		}
		else {
		    heli = data.img.heli[0][equip];
		    diff = 0;
		}
		context.save(); 
		context.translate( (2*y+1) * data.tileSize/2, (2*x+1) * data.tileSize/2);
		context.rotate(rot[ori] - (frames*diff*PI) / (2*FRAMES_PER_ROUND));
		context.drawImage(heli, -5*data.tileSize/2, -5*data.tileSize/2);
		context.restore();
	    }
	}
    }
}


function correctX (oldX, newX) {
    return oldX + ((newX-oldX) * frames) / FRAMES_PER_ROUND;
}

function correctY (oldY, newY) {
    return oldY + ((newY-oldY) * frames) / FRAMES_PER_ROUND;
}

function drawBackground (round, row) {
    var ctx = canvas.getContext('2d');
    var rectSize = data.tileSize;
    for (var i = 0; i < data.nb_cols; ++i) {
	randomNumber = (125 * randomNumber + 1) % 4096;
	var grass = data.img.grass[randomNumber % 5]; 
	ctx.drawImage(grass, i*rectSize, row*rectSize);
        var type = data.rounds[round].rows[row][i];
	if (type != '.' && type != ':') { 
	    // Grass is already there.
	    // Trees are foreground.
            var tile = selectTile(round, type, row, i);
            ctx.drawImage(tile, i*rectSize, row*rectSize);
	}
    }
}


function drawForest (round, row) {
    var ctx = canvas.getContext('2d');
    var rectSize = data.tileSize;
    for (var i = 0; i < data.nb_cols; ++i) {
        var type = data.rounds[round].rows[row][i];
	if (type == ':') { // Tree
            ctx.drawImage(data.img.tree, i*rectSize, row*rectSize);
	}
    }
}


function selectTile (round, type, row, col) {
    switch (type) {
    case 'O': // Water
        return data.img.water;
        break;
    case 'X': // Mountain
        return data.img.mountain;
        break;
    default:
        break;
    }
}

// *********************************************************************
// Button events
// *********************************************************************

function playButton () {
//    if (actRound >= data.nb_rounds - 1) actRound = 0;
    gamePaused = false;
}

function pauseButton () {
    gamePaused = true;
    gamePreview = true; //To call render again
    frames = 0;
}

function startButton () {
    actRound = 0;
    frames = 0;
    gamePreview = true;
    gamePaused = true;
}

function endButton () {
    actRound = data.nb_rounds;
    frames = 0;
    gamePreview = true;
}

function animButton () {
    gameAnim = !gameAnim;
}

function closeButton () {
    window.close();
}





// *********************************************************************
// Keyboard and Mouse events
// *********************************************************************

function onDocumentMouseWheel (event) {
}

function onDocumentKeyDown (event) {
}

function onDocumentKeyUp (event) {

    // http://www.webonweboff.com/tips/js/event_key_codes.aspx

    switch (event.keyCode) {

    case 36: // Start
        actRound = 0;
        frames = 0;
        gamePreview = true;
        break;

    case 35: // End
        actRound = data.nb_rounds;
        frames = 0;
        gamePreview = true;
        break;

    case 33: // PageDown
        actRound -= 10;
        frames = 0;
        gamePreview = true;
        break;

    case 34: // PageUp
        actRound += 10;
        frames = 0;
        gamePreview = true;
        break;

    case 38: // ArrowUp
    case 37: // ArrowLeft
        gamePaused= true;
        frames = 0;
        --actRound;
        gamePreview = true;
        break;

    case 40: // ArrowDown
    case 39: // ArrowRight
        gamePaused = true;
        frames = 0;
        ++actRound;
        gamePreview = true;
        break;

    case 32: // Space
        if (gamePaused) playButton();
        else pauseButton();
        break;

    case 72: // "h"
        help();
        break;

    default:
        //$("#debug").html(event.keyCode);
        break;
    }
}




function onWindowResize (event) {

    //Constants
    var header_height = 130;
    var canvas_margin = 20;

    // set canvas size
    var size = Math.min(document.body.offsetWidth, document.body.offsetHeight - header_height) - canvas_margin*2;

    canvas.width  = size;
    canvas.height = size;

    var max_dimension = Math.max(data.nb_cols, data.nb_rows);
    tileSize = size / max_dimension;

    drawGame();

}


function help () {
    // opens a new popup with the help page
    var win = window.open('help.html' , 'name', 'height=400, width=300');
    if (window.focus) win.focus();
    return false;
}



// *********************************************************************
// This function is called periodically.
// *********************************************************************

function mainloop () {

    // Configure buttons
    if (gamePaused) {
        $("#but_play").show();
        $("#but_pause").hide();
    } else {
        $("#but_play").hide();
        $("#but_pause").show();
    }

    if (actRound < 0) actRound = 0;
    if (actRound > data.nb_rounds) {
        actRound = data.nb_rounds;
        gamePaused = true;
        frames = 0;
    }

    if (!gamePaused || gamePreview) {

        updateGame();
        drawGame();
        writeGameState();

        if (gamePreview) {
            frames = 0;
            gamePreview = false;
        } else {
            frames++;
            if (frames == FRAMES_PER_ROUND) {
                actRound += gameDirection;
                frames = 0;
            }
        }

    }
    // periodically call mainloop
    var frame_time = 1000/speed;
    setTimeout(mainloop, frame_time);
}





// *********************************************************************
// Main function, it is called when the document is ready.
// *********************************************************************

function init () {

    // get url parameters
    var game;
    if (getURLParameter("sub") != null) {
        var domain = window.location.protocol + "//" + window.location.host;
        if (getURLParameter("nbr") != null) {
            game = domain + "/?cmd=lliuraments&sub="+getURLParameter("sub")+"&nbr="+getURLParameter("nbr")+"&download=partida";
        } else {
            game = domain + "/?cmd=partida&sub="+getURLParameter("sub")+"&download=partida";
        }
    } else {
        game = getURLParameter("game");
    }

    if (game == null || game == "") {
        // ask the user for a game input
        var inputdiv = document.getElementById('inputdiv')
        inputdiv.style.display = "";
        document.getElementById('file').addEventListener('change', function(evt) {
            //http://www.html5rocks.com/en/tutorials/file/dndfiles/
            var file = evt.target.files[0];
            var reader = new FileReader();
            reader.readAsText(file);
            reader.onloadend = function(evt) {
                if (evt.target.readyState == FileReader.DONE) { // DONE == 2
                    inputdiv.style.display = "none";
                    document.getElementById("loadingdiv").style.display="";
                    initGame(reader.result);
                } else {
                    alert("Error accessing file");
                }
            };
        }, false);
    } else {
        document.getElementById("loadingdiv").style.display="";
        // load the given game
        loadFile(game, initGame);
    }

}
