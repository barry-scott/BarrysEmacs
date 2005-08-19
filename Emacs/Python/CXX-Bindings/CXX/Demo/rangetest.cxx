#ifdef _MSC_VER
// disable warning C4786: symbol greater than 255 character,
// nessesary to ignore as <map> causes lots of warning
#pragma warning(disable: 4786)
#endif

#include "CXX/Extensions.hxx"
#include "range.hxx"

// This test also illustrates using the Py namespace explicitly

extern void debug_check_ref_queue();


std::string test_extension_object() 
	{ 
	debug_check_ref_queue();

	Py::Tuple a; // just something that isn't an range...

	Py::ExtensionObject<range> r1( new range(1, 20, 3) );
	if(range::check(a))
		std::cout << "range::check failed (1).";
	if(!range::check(r1))
		return "r::check failed (2).";

	debug_check_ref_queue();

	RangeSequence r2(1, 10, 2);
	if(r2[1] != Py::Int(3))
		return "RangeSequence check failed. ";

	debug_check_ref_queue();

	// calling an extension object method using getattr
	Py::Callable w(r2.getAttr("amethod"));
	Py::Tuple args(1);
	Py::Int j(3);
	args[0]=j;
	Py::List answer(w.apply(args));
	if(answer[0] != r2)
		return ("Extension object test failed (1)");

	if(answer[1] != args[0])
		return ("Extension object test failed (2)");

	debug_check_ref_queue();

	Py::Tuple nv(3);
	nv[0] = Py::Int(1);
	nv[1] = Py::Int(20);
	nv[2] = Py::Int(3);
	Py::Tuple unused;
	Py::List r2value;
	r2.assign(unused, nv);
	r2value = r2.value(unused);
	if(r2value[1] != Py::Int(4))
		return("Extension object test failed. (3)");

	debug_check_ref_queue();

	// repeat using getattr
	w = r2.getAttr("assign");
	Py::Tuple the_arguments(2);
	the_arguments[0] = unused;
	the_arguments[1] = nv;
	w.apply(the_arguments);

	debug_check_ref_queue();

	w = r2.getAttr("value");
	Py::Tuple one_arg(1);
	one_arg[0] = unused;
	r2value = w.apply(one_arg);
	if(r2value[1] != Py::Int(4))
		return("Extension object test failed. (4)");

	debug_check_ref_queue();
		{
		Py::ExtensionObject<range> rheap( new range(1, 10, 2) );

		debug_check_ref_queue();

		// delete rheap
		}

	debug_check_ref_queue();

	return "ok.";
	}
