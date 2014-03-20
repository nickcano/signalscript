class Foo
{
	var1;
	Foo();
	Speak();
}

function Foo::Foo()
{
	var1 = "lol ";
}

function Foo::Speak()
{
	localvar = "haha";
	if (localvar != nil)
	print(var1);
	print(localvar);
}


function testWhile()
{
	print("Expect 'haahaahaa'\n ");
	n = 0;
	while (true)
	{
		n = n + 1;
		print("h");

		i = 0;
		while (i < 2)
		{
			i = i + 1;
			print("a");
		}

		if (n == 3)
			break;
		else
			continue;
	}
	print("\n");
}

function testFor()
{
	print("Expect '012345 -- 54321 -- 13579'\n ");
	for (i = 0; i <= 5; i = i + 1)
	{
		print(i);
	}
	print(" -- ");
	for (i = 5; true; i = i - 1)
	{
		print(i);
		if (i == 1)
			break;
	}
	print(" -- ");
	for (i = 1; i < 10; i = i + 1)
	{
		if (i == 2 || i == 4 || i == 6 || i == 8)
			continue;
		print(i);
	}
	print("\n");
}

function testIncDec()
{
	print("Expect '01021 | -1 | 4'\n ");
	var = 0;
	print(var);

	var++;
	print(var);

	var--;
	print(var);

	var++;var++;
	print(var);

	var--;
	print(var);

	print(" | ");

	var--;var--;
	print(var);

	print(" | ");

	val = 1;
	val = val + val++;
	print(val);

	print("\n");
}

function testSwitch(value)
{
	switch (value)
	{
		case "nipples":
		{
			print("nipples");
			break;
		}
		case 3:
			print("three, ");
		case 2:
		{
			print("two, ");
		}
		case 1:
		{
			print("one\n");
			break;
		}
		default:
		{
			print("other\n");
		}
	}
}

function main()
{
	testWhile();
	testFor();
	testIncDec();

	print("Expect 'one'\n ");
	testSwitch(1);
	print("Expect 'two, one'\n ");
	testSwitch(2);
	print("Expect 'three, two, one'\n ");
	testSwitch(3);
	print("Expect 'other'\n ");
	testSwitch(4);
	print("Expect 'nipples'\n ");
	testSwitch("nipples");
}