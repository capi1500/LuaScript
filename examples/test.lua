int = -2351
unsigned = 135
double = 725.24735
bool = true
string = "hello world! I'm LUA"

vec = {
	{v1 = 5, v2 = "hello world!", v3 = {5, 4, 3, 2, 1}},
	"another element",
	74269
}

function write(sth)
	print(sth)
end

function sum(a, b)
	return a + b
end

function callCFun()
	cfun()
end
