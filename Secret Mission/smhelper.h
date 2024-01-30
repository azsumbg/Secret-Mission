#pragma once

#ifdef SMHELPER_EXPORTS
#define SMHELPER_API _declspec(dllexport)
#else
#define SMHELPER_API _declspec(dllimport)
#endif

#define scr_width 1016.0f
#define scr_height 839.0f

enum class DLRESULT { ok = 0, fail = 1, not_supported = 2 };

enum class dirs { up = 0, down = 1, left = 2, right = 3, stop = 4 };
enum class positions { up = 0, down = 1, no_position = 2, fly = 3 };

enum class types {
	no_type = 0, field = 1, ship = 2, big_block_u = 3, big_block_d = 4,
	small_block = 5, vblock = 6, portal = 7, battleship = 8, cloud1 = 9,
	cloud2 = 10, laser = 11, bad_laser = 12, explosion = 13
};


class SMHELPER_API BASIC
{
	protected:
		float width = 0;
		float height = 0;

	public:
		float x = 0;
		float y = 0;
		float ex = 0;
		float ey = 0;

		BASIC(float _x, float _y, float _width = 1.0f, float _height = 1.0f)
		{
			x = _x;
			y = _y;
			width = _width;
			height = _height;
			ex = x + width;
			ey = y + height;
		}

		virtual ~BASIC() {};
		virtual void Release()
		{
			delete this;
		}
		void SetEdges()
		{
			ex = x + width;
			ey = y + height;
		}
		void NewDims(float _width, float _height)
		{
			width = _width;
			height = _height;
			ex = x + width;
			ey = y + height;
		}
};

class SMHELPER_API PROTON :public BASIC
{
	protected:
		int frame_delay = 0;
		int max_frames = 0;
		int frame = 0;
		types type = types::no_type;
		float speed = 0;
		positions position = positions::no_position;

	public:
		int lifes = 0;
		dirs dir = dirs::stop;
		
		PROTON(types _type, float _x, float _y, float _width, float _height) :BASIC(_x, _y, _width, _height)
		{
			type = _type;
		}
		virtual ~PROTON() {};
		virtual void Release()
		{
			delete this;
		}
		types GetType()const
		{
			return type;
		}
		positions GetPos()const
		{
			return position;
		}
		void SetPos(positions _which)
		{
			position = _which;
		}

		virtual int GetFrame() = 0;
		virtual DLRESULT Transform(types _whichtype) = 0;
		virtual DLRESULT Move(float _gear = 1.0f) = 0;
};

typedef BASIC* bas_ptr;
typedef PROTON* prot_ptr;

extern SMHELPER_API prot_ptr ProtonFactory(types _what, float _x, float _y);