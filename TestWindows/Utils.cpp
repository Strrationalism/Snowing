#include "pch.h"
#include <PropParser.h>

TEST(Utils, Prop)
{
	auto kvb = LoadAsset("PropTest.txt");
	PropParser<wchar_t> p{ &kvb };
	
	if (p.Get<int>(L"fnmdp") != 16)
		testing::AssertionFailure();
	
	if (!(p.Get<float>(L"assFucker") > 12.33f && p.Get<float>(L"assFucker") < 12.35f))
		testing::AssertionFailure();

	if (p.Get(L"nmsl") != L"123  ")
		testing::AssertionFailure();

	if (p.Get(L"nmsl1") != L"SuperMario")
		testing::AssertionFailure();

	if (p.Get(L"nmsl2") != L"Kuppa")
		testing::AssertionFailure();

	if (p.Get(L"nmsl3") !=  L"Test String" )
		testing::AssertionFailure();

	if (p.Get(L"nmsl4") !=  L"\"123\"" )
		testing::AssertionFailure();

	if (p.Get(L"nmsl5") !=  L"      " )
		testing::AssertionFailure();

	if (p.Get<int>(L"assFucker2") != 999999)
		testing::AssertionFailure();

	if (!(p.Get<float>(L"assfc") > 1225.9995f && p.Get<float>(L"assFucker") < 1225.9997f))
		testing::AssertionFailure();
}

TEST(Utils, Log)
{
	Snowing::Log<wchar_t>(12345,12345,678910.0f,L"测试");
	Snowing::Log("TEST2");
}

TEST(Utils, ObjectPool)
{
	class PooledObj final : public Snowing::MemPool<PooledObj>
	{
	private:
		int i = 100;
	public:
		PooledObj()
		{
			Snowing::Log(L"Con");
		}

		~PooledObj()
		{
			Snowing::Log(L"Dec");
		}
	};

	auto p = new PooledObj;
	auto p2 = new PooledObj;
	delete p;
	p = new PooledObj;
	delete p;
	delete p2;
}

TEST(Utils, CSV)
{
	auto p = LoadAsset("CSVTest.csv");
	CSVParser<char> csv{ &p };

	do
	{
		Log("Line:",csv.CurrentLine());
		for (int i = 0; i < 3; ++i)
			Log(csv.Pop());
	} while (csv.NextLine());
}

TEST(Utils, SingleInstance)
{
	class SI : public SingleInstance<SI>
	{
	public:
		int i = 0;
	};

	{
		SI si;
		si.i = 1;
		SI::Get().i = 2;

		SI si2{ std::move(si) };
		SI::Get().i = 3;

		assert(si.i == 2);
		assert(si2.i == 3);
	}
}


TEST(Utils, Reinit)
{
	auto blob = LoadAsset("CSVTest.csv");
	Reinit(blob, LoadAsset("PropTest.txt"));

	PropParser<wchar_t> p{ &blob };

	if (p.Get<int>(L"fnmdp") != 16)
		testing::AssertionFailure();
}

TEST(Utils, LoadFontFix)
{
	auto p = Snowing::Graphics::FontSprite::LoadCharfixFromCSV(LoadAsset("FontFix.csv"));
	assert(p[L'好'].x > 9.9f && p[L'好'].x < 10.1f);
	assert(p[L'好'].y > 10.9f && p[L'好'].y < 11.1f);
	assert(p[L'孬'].x > -10.1f && p[L'孬'].x < -9.9f);
	assert(p[L'孬'].y > -11.1f && p[L'孬'].y < -10.9f);
}

TEST(Utils, BinaryWriterReader)
{
	using namespace std;
	BlobWriter w;
	w.Write(1);
	w.Write(1.0f);
	w.WriteString("NMSL"sv);
	w.WriteString(L"道路千万条"sv);
	w.Write(11111);
	w.Write(1.5);
	w.Write(Math::Vec2<int>{999, 888});
	w.Write(Math::Vec4<int>{777, 666, 555, 444});

	constexpr auto FileName = "UtilsTest-BinaryWriterReader.tmp";

	{
		std::ofstream out(FileName,std::ios::binary);
		out.write(w.Borrow().Get<char*>(), w.Borrow().Size());
	}

	auto testReader = [](BlobReader& r)
	{
		Assert(r.Read<int>(), 1);
		Assert(r.Read<float>(), 1.0f);
		Assert(r.ReadString<char>(), "NMSL"sv);
		Assert(r.ReadString<wchar_t>(), L"道路千万条"sv);
		Assert(r.Read<int>(), 11111);
		Assert(r.Read<double>(), 1.5);
		Assert(r.Read<Math::Vec2<int>>(), Math::Vec2<int>{999, 888});
		Assert(r.Read<Math::Vec4<int>>(), Math::Vec4<int>{777, 666, 555, 444});
	};

	auto p1 = w.Borrow();
	testReader(Snowing::BlobReader{ &p1 });

	{
		Snowing::BlobReader p1r2{ &p1 };
		auto borrowedSlice = p1r2.BorrowSlice(p1.Size());
		testReader(Snowing::BlobReader{ &borrowedSlice });
	}

	auto p2 = w.Copy();
	testReader(Snowing::BlobReader{ &p2 });

	auto p3 = LoadAsset(FileName);
	testReader(Snowing::BlobReader{ &p3 });
}

TEST(Utils, ConvertCoordinateOfPosition)
{
	Math::Coordinate2DRect coord
	{
		{0.0f,0.0f},
		{800.0f,600.0f}
	};

	Math::Vec2f position{ 400.0f,300.0f };
	auto pos = Math::ConvertPosition2DCoordinate(position, coord, 
		Input::MousePosition::CoordinateSystem);
	Log("Output:", pos.x, pos.y);
}
