
BEmacsClassMonikerps.dll: dlldata.obj BEmacsClassMoniker_p.obj BEmacsClassMoniker_i.obj
	link /dll /out:BEmacsClassMonikerps.dll /def:BEmacsClassMonikerps.def /entry:DllMain dlldata.obj BEmacsClassMoniker_p.obj BEmacsClassMoniker_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del BEmacsClassMonikerps.dll
	@del BEmacsClassMonikerps.lib
	@del BEmacsClassMonikerps.exp
	@del dlldata.obj
	@del BEmacsClassMoniker_p.obj
	@del BEmacsClassMoniker_i.obj
