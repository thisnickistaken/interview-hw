function send_ajax(location, type, ajax, handler, params)
{
	ajax.open(type, location, true);
	ajax.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
	ajax.setRequestHeader("Content-length", params.length);
	ajax.setRequestHeader("Connection", "close");
	ajax.onreadystatechange = handler;
	ajax.send(params);
}
