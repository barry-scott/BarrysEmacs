#ifdef _MSC_VER
// disable warning C4786: symbol greater than 255 character,
// nessesary to ignore as <map> causes lots of warning
#pragma warning(disable: 4786)
#endif

#include "range.hxx"
// Connect range objects to Python


Py::Object range::repr()
	{
	return Py::String(asString());
	}

int range::sequence_length()
	{
	return length();
	}

Py::Object range::sequence_item(int i) 
	{
	return Py::Int(item(i));
	}

Py::Object range::sequence_concat( const Py::Object &j)
	{
	Py::Int k(j);
	return Py::asObject(extend(int(k)));
	}

Py::Object range::sequence_slice(int i, int j)
	{
	return Py::asObject(slice(i,j));
	}


Py::Object range::getattr( const char *name )
	{
	if(std::string(name) == "c")
		return Py::Float(300.0);

	if(std::string(name) == "start")
		return Py::Int(start);

	return getattr_methods( name );
	}

// "regular" methods...
Py::Object range::amethod( const Py::Tuple &t ) 
	{
	t.verify_length(1);
	Py::List result;
	result.append(Py::Object(this));
	result.append(t[0]);

	return result;
	}

Py::Object range::value( const Py::Tuple &t )
	{
	return c_value(t);
	}

Py::Object range::assign( const Py::Tuple &t ) 
	{
	t.verify_length(2);

	Py::Tuple t1(t[0]); // subscripts
	Py::Object o2(t[1]); // rhs;
	c_assign (t1, o2);

	return Py::Nothing();
	}

void range::init_type()
	{
	behaviors().name("range");
	behaviors().doc("range objects: start, stop, step");
	behaviors().supportRepr();
	behaviors().supportGetattr();
	behaviors().supportSequenceType();

	add_varargs_method("amethod", &range::amethod, "demonstrate how to document amethod");
	add_varargs_method("assign", &range::assign);
	add_varargs_method("value", &range::value);
	add_varargs_method("reference_count", &range::reference_count);
	}
