'use strict';
var service = "http://maxwellden.pythonanywhere.com/coffe";

const pushButton = document.querySelector('#btn-vip-coffee');

let isSubscribed = false;
let swRegistration = null;

function urlB64ToUint8Array(base64String) {
	const padding = '='.repeat((4 - base64String.length % 4) % 4);
	const base64 = (base64String + padding)
		.replace(/\-/g, '+')
		.replace(/_/g, '/');

	const rawData = window.atob(base64);
	const outputArray = new Uint8Array(rawData.length);

	for (let i = 0; i < rawData.length; ++i) {
		outputArray[i] = rawData.charCodeAt(i);
	}
	return outputArray;
}

function updateBtn() {
	if (Notification.permission === 'denied') {
		pushButton.textContent = 'Notificações bloqueadas.';
		pushButton.disabled = true;
		return;
	}

	if (isSubscribed) {
		pushButton.textContent = 'Não me avisar do café novinho';
	} else {
		pushButton.textContent = 'Me avise quando tiver café novinho';
	}

	pushButton.disabled = false;
}

function subscribeUser() {
	const applicationServerPublicKey = localStorage.getItem('applicationServerPublicKey');
	const applicationServerKey = urlB64ToUint8Array(applicationServerPublicKey);

	swRegistration.pushManager.subscribe({
			userVisibleOnly: true,
			applicationServerKey: applicationServerKey
		})
		.then(function(subscription) {
			console.log('User is subscribed.');
			console.log('subscription', subscription);

			localStorage.setItem('sub_token',JSON.stringify(subscription));
			isSubscribed = true;

			push_token();
			updateBtn();
		})
		.catch(function(err) {
			console.log('Failed to subscribe the user: ', err);
			updateBtn();
		});
}

function unsubscribeUser() {
	swRegistration.pushManager.getSubscription()
		.then(function(subscription) {
			if (subscription) {
				return subscription.unsubscribe();
			}
		})
		.catch(function(error) {
			console.log('Error unsubscribing', error);
		})
		.then(function() {
			console.log('User is unsubscribed.');
			isSubscribed = false;

			updateBtn();
		});
}

function initializeUI() {
	pushButton.addEventListener('click', function() {
		pushButton.disabled = true;
		if (isSubscribed) {
			unsubscribeUser();
		} else {
			subscribeUser();
		}
	});

	// Set the initial subscription value
	swRegistration.pushManager.getSubscription()
		.then(function(subscription) {
			isSubscribed = !(subscription === null);

			if (isSubscribed) {
				console.log('User IS subscribed.');
			} else {
				console.log('User is NOT subscribed.');
			}

			updateBtn();
		});
}

if ('serviceWorker' in navigator && 'PushManager' in window) {
	console.log('Service Worker and Push is supported');

	navigator.serviceWorker.register("/static/sw.js")
		.then(function(swReg) {
			console.log('Service Worker is registered', swReg);

			swRegistration = swReg;
			initializeUI();
		})
		.catch(function(error) {
			console.error('Service Worker Error', error);
		});
} else {
	console.warn('Push meapplicationServerPublicKeyssaging is not supported');
	pushButton.textContent = 'Seu navegador não suporta notificações.';
}

function push_token() {
	console.log("sub_token", localStorage.getItem('sub_token'));

	$.ajax({
		type: "POST",
		url: "/subscription/",
		contentType: 'application/json; charset=utf-8',
		dataType:'json',
		data: JSON.stringify({'sub_token': localStorage.getItem('sub_token')}),

		success: function(data){
			console.log("success", data);
    	},

		error: function(jqXhr, textStatus, errorThrown ){
        	console.log("error", errorThrown);
    	}
	});
}

function toggleClass(element, toggleClass){
	var currentClass = element.className;
	var newClass;
	if(currentClass.split(" ").indexOf(toggleClass) > -1){ //has class
		newClass = currentClass.replace(new RegExp('\\b'+toggleClass+'\\b','g'),"")
	}
	else {
		newClass = currentClass + " " + toggleClass;
  	}

  	element.className = newClass.trim();
}

function toggleBottle(new_class) {
	var bottle = document.getElementById("bottle");
	bottle.className = "bottle";
	toggleClass(bottle, new_class)
}

function loadData() {
  $.getJSON(service, function(data) {
		if (data["fill"] < 0) {
			$("#fill").html("Garrafa fora do lugar.");
		}
		else if (data["fill"] < 100) {
			$("#fill").html("Vazio.");
		}
		else {
	  		$("#fill").html(data["fill"].toFixed(2) + "mL");
		}

		if (data["fill"] < 150) {
			toggleBottle("");
		}
		else if (data["fill"] >= 2500) {
	  		$("#fill").html("Alguém está utilizando a garrafa.");
		}
		else if (data["fill"] >= 850) {            
	  		toggleBottle("bottle_full");
		}
		else if (data["fill"] >= 630) {
	  		toggleBottle("bottle_third");
		}
		else if (data["fill"] >= 315) {
	  		toggleBottle("bottle_half");            
		}          
		else {
	  		toggleBottle("bottle_quarter");
		}
  	});
}

$(document).ready(function(){
	loadData();
	$.ajax({
		type:"GET",
		url:'/subscription/',
		success:function(response){
			console.log("response",response);
			localStorage.setItem('applicationServerPublicKey', response.public_key);
		}
	})
});
