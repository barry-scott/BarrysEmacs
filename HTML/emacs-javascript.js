//
//	emacs-javascript.js
//	Copyright (c) 1998 Barry A. Scott
//
var target_index;
var target_anchor;

function mouseOver( link )
	{
	window.status = " ";
	return true;
	}

function setupLinks( w )
	{
	for( i=0; i<document.links.length; i++ )
		document.links[i].onMouseOver=mouseOver;
	}

function initialLoadSearchHandling()
	{
	if( top.location.hash != "" )
		{
		var hash = new String(top.location.hash);
		var lesshash = hash.substring(1);
		var parts = lesshash.split("&");
		if( parts.length == 1 )
			loadDocument( parts[0] );
		else if( parts.length == 2 )
			loadDocument( parts[0], "#"+parts[1] );
		else
			alert("URL coantains too many parts");
		}
	}

function loadDocument( doc, hash )
	{
//	alert('loadDocument('+doc+","+hash+")");
	if( arguments.length < 1 || arguments.length > 2 )
		{
		alert('loadDocument() expects 1 or 2 args but was called with '+arguments.length);
		return false;
		}

	if( doc == "ug" || doc == "pg" || doc == "extn" )
		loadDocumentSimple( doc, hash );
	else if( doc == "mlisp" || doc == "var" || doc == "fn" )
		loadDocumentMLispRef( doc, hash );
	else
		alert('loadDocument() cannot load a doc type of '+doc);
	return false;
	}

function loadDocumentSimple( doc, hash )
	{
	// assume the index load handler has no work to do
	target_index = null;
	target_anchor = null;

	var list_src;
	var body_src;

	if( doc == "ug" )
		{
		list_src = "ug_index.html";
		body_src = "ug_top.html";
		}
	else if( doc == "pg" )
		{
		list_src = "pg_index.html";
		body_src = "pg.html";
		}
	else if( doc == "extn" )
		{
		list_src = "extn_index.html";
		body_src = "extn_intro.html";
		}
	else
		{
		alert('loadDocumentSimple() cannot load doc type of '+doc);
		return false;
		}

	// see if we have the hash parameter
	if( hash != null )
		{
		body_src = "about:blank";
		target_index = list_src;
		target_anchor = hash;
		}

	var d = top.doc_window.document

	d.open();
	d.write('<html>');
	d.write('<FRAMESET COLS="30%,*">');
	d.write('<FRAME SRC="'+list_src+'" NAME="list" MARGINWIDTH="3" MARGINHEIGHT="3"></FRAME>');
	d.write('<FRAME SRC="'+body_src+'" NAME="body" MARGINWIDTH="3" MARGINHEIGHT="3"></FRAME>');
	d.write('</FRAMESET>');
	d.write('</html>');
	d.close();

	return false;
	}

function loadDocumentMLispRef( doc, hash )
	{
	// assume the index load handler has no work to do
	target_index = null;
	target_anchor = null;

	var list_src;
	var body_src;

	if( doc == "mlisp" )
		{
		if( hash != null )
			{
			alert('loadDocumentMLispRef("mlisp") cannot use hash of '+hash);
			return false;
			}
		list_src = "about:blank";
		body_src = "fn_front.html";
		}
	else if( doc == "var" )
		{
		if( hash == null )
			{
			alert('loadDocumentMLispRef("var") must have a hash parameter');
			return false;
			}
		list_src = "var_list.html";
		body_src = "about:blank";
		target_index = list_src;
		target_anchor = hash;
		}
	else if( doc == "fn" )
		{
		if( hash == null )
			{
			alert('loadDocumentMLispRef("fn") must have a hash parameter');
			return false;
			}
		list_src = "fn_list.html";
		body_src = "about:blank";
		target_index = list_src;
		target_anchor = hash;
		}
	else
		{
		alert('loadDocumentMLispRef() cannot load doc type of '+doc);
		return false;
		}

	var d = top.doc_window.document

	d.open();
	d.write('<html>');
	d.write('<FRAMESET COLS="30%,*">');
	d.write('<FRAMESET ROWS="80,80,*">');
	d.write('<FRAME scrolling="no" SRC="functions.html" NAME="functions" MARGINWIDTH="3" MARGINHEIGHT="3"></FRAME>');
	d.write('<FRAME scrolling="no" SRC="variables.html" NAME="variables" MARGINWIDTH="3" MARGINHEIGHT="3"></FRAME>');
	d.write('<FRAME src="'+list_src+'" NAME="list" MARGINWIDTH="3" MARGINHEIGHT="3" SCROLLING="YES"></FRAME>');
	d.write('</FRAMESET>');
	d.write('<FRAME SRC="'+body_src+'" NAME="body" MARGINWIDTH="3" MARGINHEIGHT="3" SCROLLING="YES"></FRAME>');
	d.write('</FRAMESET>');
	d.write('</html>');
	d.close();

	return false;
	}


function indexLoadedHandler( w )
	{
	//alert('indexLoadedHandler() target_anchor='+target_anchor);
	// if target_anchor is null then no special handling required
	if( target_anchor == null )
		return;

	// double check that the user has not aborted the load and clicked on another link
	var htm_filename = new String(w.location.pathname);
		var a = htm_filename.split('\\');
	htm_filename = a[a.length-1];
		a = htm_filename.split('/');
	htm_filename = a[a.length-1];

	if( target_index != htm_filename )
		{
		//alert('indexLoadedHandler() override target_index='+target_index+" htm_filename="+htm_filename);
		return;
		}

	// do special navigation

	var l = w.document.links;
	for(i=0; i<l.length; i++ )
		if( l[i].hash == target_anchor )
			{
			top.doc_window.body.location.href = l[i];
			return;
			}
	alert('Cannot find the link for anchor '+target_anchor);

	// turn off special navigation
	target_anchor = null;
	}

function indexExperiments( w )
	{
	var l = w.document.links;
	var a = w.document.anchors;
	var linkinfo = window.open("about:blank","linkinfo");
	var d = linkinfo.document;
	d.open();
	d.write('<html><pre>');
	d.writeln("Number of links "+l.length);
	for(i=0; i<l.length; i++ )
		d.writeln("     "+l[i].hash);
	d.write('</pre></html>');
	d.close();
	}
