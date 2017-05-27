
var asdfButton=document.getElementById("asdf");
var letterButton=document.getElementById("letter");

var letterMatrix;
var letterMatrixZero=['A','B','D','H','P'];
var currentBin;
var displayString=''; // cap this at a certain number of characters
var currentLetter='';
var checkingForRelease;


// on key press, show the tree and update the currentLetter being pressed. Also update display.
// on key release, key in the letters that are down, or rather the last pressed letter
// if it's backspace (8) then you need to delete the last character from the displayString

// on release of any special key, log the potential character and 
//    check for subsequent release to currentBin in the next 100ms
// otherwise, update to the pertinent letterMatrix

document.onkeyup = function(key) {
	if ((key.keyCode)==32) {
		if (checkingForRelease=='' || checkingForRelease==undefined) {setAtRelease(0);}
	}
	else if ((key.keyCode)==70) {
		if (checkingForRelease=='' || checkingForRelease==undefined) {setAtRelease(1);}
	}
	else if ((key.keyCode)==68) {
		if (checkingForRelease=='' || checkingForRelease==undefined) {setAtRelease(2);}
	}
	else if ((key.keyCode)==83) {
		if (checkingForRelease=='' || checkingForRelease==undefined) {setAtRelease(3);}
	}
	else if ((key.keyCode)==65) {
		if (checkingForRelease=='' || checkingForRelease==undefined) {setAtRelease(4);}
	}
}

document.onkeydown = function(key) {
	if (currentBin==undefined) {
		resetKeys();
	}
	if ((key.keyCode)==32) {
		currentBin=currentBin.replaceAt(4,"1");
		showLetterPreview(numberToLetter(parseInt(currentBin,2)));
		showTree(currentBin);
	}
	else if ((key.keyCode)==70) {
		currentBin=currentBin.replaceAt(3,"1");
		showLetterPreview(numberToLetter(parseInt(currentBin,2)));
		showTree(currentBin);
	}
	else if ((key.keyCode)==68) {
		currentBin=currentBin.replaceAt(2,"1");
		showLetterPreview(numberToLetter(parseInt(currentBin,2)));
		showTree(currentBin);
	}
	else if ((key.keyCode)==83) {
		currentBin=currentBin.replaceAt(1,"1");
		showLetterPreview(numberToLetter(parseInt(currentBin,2)));
		showTree(currentBin);
	}
	else if ((key.keyCode)==65) {
		currentBin=currentBin.replaceAt(0,"1");
		showLetterPreview(numberToLetter(parseInt(currentBin,2)));
		showTree(currentBin);
	}
	// if backspace, then do backspace
	else if ((key.keyCode)==8) {
		displayString=displayString.substring(0,displayString.length-1);
		var displayDiv=document.getElementById("displayText");
		displayDiv.innerText=displayString;
	}
		console.log(currentBin);


}

String.prototype.replaceAt=function(index, replacement) {
    return this.substr(0, index) + replacement+ this.substr(index + replacement.length);
}

function setAtRelease(index) {
	console.log('bin is now'+currentBin);
	checkingForRelease=currentBin;
	currentLetter=numberToLetter(parseInt(currentBin,2));
	var releaseTimer = setTimeout(function(){
		updateDisplay();
		resetKeys();
		checkingForRelease='';
	},200);
}

function checkForRelease(index) {
	// to be called on keyup
	// set variable
	checkingForRelease=currentBin;
	console.log('at time of first release: '+currentBin);
	currentBin=currentBin.replaceAt(4-index,'0');
	var releaseTimer = setTimeout(function(){
		// check currentBin
		if (currentBin=='00000' || currentBin==checkingForRelease) {
			// if all keys have been released, then log the checkingForRelease as the actual letter
			currentLetter=numberToLetter(parseInt(checkingForRelease,2));
			// log current letter into display
			updateDisplay();
			resetKeys();
		}
		else {
			// other keys not released, so simply update the visual tree
			console.log('bin is now '+currentBin);
			showTree(currentBin);
			displayLetters();
		}
		checkingForRelease='';
	},200)
}

function updateDisplay() {
	if (currentLetter!=undefined) {
		console.log('adding '+currentLetter);
		// scoot the currentLetter to the displayString
		displayString=displayString+currentLetter;
		// doublecheck and clip display if it is >20char
		if (displayString.length>20) {
			displayString=displayString.substring(displayString.length-20,displayString.length);
		}
		// put the display in the div
		var displayDiv=document.getElementById("displayText");
		displayDiv.innerText=displayString;
		console.log(displayString);
	}
}

function resetKeys() {	// to run after simultaneous key release confirmed
	letterMatrix=letterMatrixZero;
	showLetterPreview('');
	displayLetters();
	currentLetter='';
	checkingForRelease='';
	currentBin='00000';
	showTree('00000');
	document.getElementById((4-0).toString()).parentElement.className='key';
	document.getElementById((4-1).toString()).parentElement.className='key';
	document.getElementById((4-2).toString()).parentElement.className='key';
	document.getElementById((4-3).toString()).parentElement.className='key';
	document.getElementById((4-4).toString()).parentElement.className='key';
}

function showTree(binStr) {
	// on keypress, get the surrounding codes
	// if 0, indicate the press if it were not 0
	// if it is 1, then make the color of the key dark
	// also do not show a letter on it, but instead show the letter in the display

	for (var x=0;x<binStr.length;x++) {
		if (binStr[x]==0) {
			// 
			var newBinStr=binStr.substring(0,x)+'1'+binStr.substring(x+1,6);
			var letter=numberToLetter(parseInt(newBinStr,2));
			if (letter!=undefined) {
				letterMatrix[4-x]=numberToLetter(parseInt(newBinStr,2));
			}
			else {
				letterMatrix[4-x]='';
			}
			
		}
		else {
			document.getElementById((4-x).toString()).parentElement.className='pressedKey';
		}
	}
	displayLetters();
}

function showLetterPreview(ltrStr) {
	var preview = document.getElementById('currentLetter');
	preview.innerText=ltrStr;
}

function displayLetters() {
	// show letterMatrix
	var aKey=document.getElementById("4");
	var sKey=document.getElementById("3");
	var dKey=document.getElementById("2");
	var fKey=document.getElementById("1");
	var spcKey=document.getElementById("0");
	aKey.innerText=letterMatrix[4];
	sKey.innerText=letterMatrix[3];
	dKey.innerText=letterMatrix[2];
	fKey.innerText=letterMatrix[1];
	spcKey.innerText=letterMatrix[0];	
}

function keyToBinary(keyCode) {
	var n=keyCode=64;
	// convert to binary
	var binStr=(+n).toString(2);
	while (binStr.length<5) {
		binStr='0'+binStr;
	}
	return binStr;
}

function letterToKey(letter){
	if (letter=="a") {
		return 65;
	}
	else if (letter=="b") {
		return 66;
	}
	else if (letter=="c") {
		return 67;
	}
	else if (letter=="d") {
		return 68;
	}
	else if (letter=="e") {
		return 69;
	}
	else if (letter=="f") {
		return 70;
	}
	else if (letter=="g") {
		return 71;
	}
	else if (letter=="h") {
		return 72;
	}
	else if (letter=="i") {
		return 73;
	}
	else if (letter=="j") {
		return 74;
	}
	else if (letter=="k") {
		return 75;
	}
	else if (letter=="l") {
		return 76;
	}
	else if (letter=="m") {
		return 77;
	}
	else if (letter=="n") {
		return 78;
	}
	else if (letter=="o") {
		return 79;
	}
	else if (letter=="p") {
		return 80;
	}
	else if (letter=="q") {
		return 81;
	}
	else if (letter=="r") {
		return 82;
	}
	else if (letter=="s") {
		return 83;
	}
	else if (letter=="t") {
		return 84;
	}
	else if (letter=="u") {
		return 85;
	}
	else if (letter=="v") {
		return 86;
	}
	else if (letter=="w") {
		return 87;
	}
	else if (letter=="x") {
		return 88;
	}
	else if (letter=="y") {
		return 89;
	}
	else if (letter=="z") {
		return 90;
	}
}

function numberToLetter(number) {
	if (number==1) {
		return 'A';
	}
	else if (number==2) {
		return 'B';
	}
	else if (number==3) {
		return 'C';
	}
	else if (number==4) {
		return 'D';
	}
	else if (number==5) {
		return 'E';
	}
	else if (number==6) {
		return 'F';
	}
	else if (number==7) {
		return 'G';
	}
	else if (number==8) {
		return 'H';
	}
	else if (number==9) {
		return 'I';
	}
	else if (number==10) {
		return 'J';
	}
	else if (number==11) {
		return 'K';
	}
	else if (number==12) {
		return 'L';
	}
	else if (number==13) {
		return 'M';
	}
	else if (number==14) {
		return 'N';
	}
	else if (number==15) {
		return 'O';
	}
	else if (number==16) {
		return 'P';
	}
	else if (number==17) {
		return 'Q';
	}
	else if (number==18) {
		return 'R';
	}
	else if (number==19) {
		return 'S';
	}
	else if (number==20) {
		return 'T';
	}
	else if (number==21) {
		return 'U';
	}
	else if (number==22) {
		return 'V';
	}
	else if (number==23) {
		return 'W';
	}
	else if (number==24) {
		return 'X';
	}
	else if (number==25) {
		return 'Y';
	}
	else if (number==26) {
		return 'Z';
	}
	else if (number==31) {
		return " ";
	}
}

//1 2 4 8 16