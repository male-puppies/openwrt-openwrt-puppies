/****************************************************************************** 
* 
* Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved. 
* 
* This program is free software; you can redistribute it and/or modify it 
* under the terms of version 2 of the GNU General Public License as 
* published by the Free Software Foundation. 
* 
* This program is distributed in the hope that it will be useful, but WITHOUT 
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for 
* more details. 
* 
* You should have received a copy of the GNU General Public License along with 
* this program; if not, write to the Free Software Foundation, Inc., 
* 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA 
* 
* 
******************************************************************************/

/*Image2HeaderVersion: 2.22*/
#include "mp_precomp.h"
#include "../phydm_precomp.h"

#if (RTL8197F_SUPPORT == 1)
static BOOLEAN
CheckPositive(
	IN  PDM_ODM_T     pDM_Odm,
	IN  const u4Byte  Condition1,
	IN  const u4Byte  Condition2,
	IN	const u4Byte  Condition3,
	IN	const u4Byte  Condition4
)
{
	u1Byte    _BoardType = ((pDM_Odm->BoardType & BIT4) >> 4) << 0 | /* _GLNA*/
				((pDM_Odm->BoardType & BIT3) >> 3) << 1 | /* _GPA*/ 
				((pDM_Odm->BoardType & BIT7) >> 7) << 2 | /* _ALNA*/
				((pDM_Odm->BoardType & BIT6) >> 6) << 3 | /* _APA */
				((pDM_Odm->BoardType & BIT2) >> 2) << 4;  /* _BT*/  

	u4Byte	cond1   = Condition1, cond2 = Condition2, cond3 = Condition3, cond4 = Condition4;
	u4Byte    driver1 = pDM_Odm->CutVersion       << 24 | 
				(pDM_Odm->SupportInterface & 0xF0) << 16 | 
				pDM_Odm->SupportPlatform  << 16 | 
				pDM_Odm->PackageType      << 12 | 
				(pDM_Odm->SupportInterface & 0x0F) << 8  |
				_BoardType;

	u4Byte    driver2 = (pDM_Odm->TypeGLNA & 0xFF) <<  0 |  
				(pDM_Odm->TypeGPA & 0xFF)  <<  8 | 
				(pDM_Odm->TypeALNA & 0xFF) << 16 | 
				(pDM_Odm->TypeAPA & 0xFF)  << 24; 

u4Byte    driver3 = 0;

	u4Byte    driver4 = (pDM_Odm->TypeGLNA & 0xFF00) >>  8 |
				(pDM_Odm->TypeGPA & 0xFF00) |
				(pDM_Odm->TypeALNA & 0xFF00) << 8 |
				(pDM_Odm->TypeAPA & 0xFF00)  << 16;

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_TRACE, 
	("===> CheckPositive (cond1, cond2, cond3, cond4) = (0x%X 0x%X 0x%X 0x%X)\n", cond1, cond2, cond3, cond4));
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_TRACE, 
	("===> CheckPositive (driver1, driver2, driver3, driver4) = (0x%X 0x%X 0x%X 0x%X)\n", driver1, driver2, driver3, driver4));

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_TRACE, 
	("	(Platform, Interface) = (0x%X, 0x%X)\n", pDM_Odm->SupportPlatform, pDM_Odm->SupportInterface));
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_TRACE, 
	("	(Board, Package) = (0x%X, 0x%X)\n", pDM_Odm->BoardType, pDM_Odm->PackageType));


	/*============== Value Defined Check ===============*/
	/*QFN Type [15:12] and Cut Version [27:24] need to do value check*/
	
	if (((cond1 & 0x0000F000) != 0) && ((cond1 & 0x0000F000) != (driver1 & 0x0000F000)))
		return FALSE;
	if (((cond1 & 0x0F000000) != 0) && ((cond1 & 0x0F000000) != (driver1 & 0x0F000000)))
		return FALSE;

	/*=============== Bit Defined Check ================*/
	/* We don't care [31:28] */

	cond1   &= 0x00FF0FFF; 
	driver1 &= 0x00FF0FFF; 

	if ((cond1 & driver1) == cond1) {
		u4Byte bitMask = 0;

		if ((cond1 & 0x0F) == 0) /* BoardType is DONTCARE*/
			return TRUE;

		if ((cond1 & BIT0) != 0) /*GLNA*/
			bitMask |= 0x000000FF;
		if ((cond1 & BIT1) != 0) /*GPA*/
			bitMask |= 0x0000FF00;
		if ((cond1 & BIT2) != 0) /*ALNA*/
			bitMask |= 0x00FF0000;
		if ((cond1 & BIT3) != 0) /*APA*/
			bitMask |= 0xFF000000;

		if (((cond2 & bitMask) == (driver2 & bitMask)) && ((cond4 & bitMask) == (driver4 & bitMask)))  /* BoardType of each RF path is matched*/
			return TRUE;
		else
			return FALSE;
	} else
		return FALSE;
}
static BOOLEAN
CheckNegative(
	IN  PDM_ODM_T     pDM_Odm,
	IN  const u4Byte  Condition1,
	IN  const u4Byte  Condition2
)
{
	return TRUE;
}

/******************************************************************************
*                           AGC_TAB.TXT
******************************************************************************/

u4Byte Array_MP_8197F_AGC_TAB[] = { 
	0x80001003,	0x00000000,	0x40000000,	0x00000000,
		0xC78, 0x0DB0001F,
		0xC78, 0x0DA0011F,
		0xC78, 0x0D90021F,
		0xC78, 0x0D80031F,
		0xC78, 0x0D70041F,
		0xC78, 0x0D60051F,
		0xC78, 0x0D50061F,
		0xC78, 0x0D40071F,
		0xC78, 0x0D30081F,
		0xC78, 0x0D20091F,
		0xC78, 0x0D100A1F,
		0xC78, 0x0D000B1F,
		0xC78, 0x0CF00C1F,
		0xC78, 0x0CE00D1F,
		0xC78, 0x0CD00E1F,
		0xC78, 0x0CC00F1F,
		0xC78, 0x0CB0101F,
		0xC78, 0x0CA0111F,
		0xC78, 0x0C90121F,
		0xC78, 0x0C80131F,
		0xC78, 0x0C70141F,
		0xC78, 0x0C60151F,
		0xC78, 0x0C50161F,
		0xC78, 0x0C40171F,
		0xC78, 0x0C30181F,
		0xC78, 0x0C20191F,
		0xC78, 0x0C101A1F,
		0xC78, 0x0A501B1F,
		0xC78, 0x0A401C1F,
		0xC78, 0x0A301D1F,
		0xC78, 0x0A201E1F,
		0xC78, 0x0A101F1F,
		0xC78, 0x0A00201F,
		0xC78, 0x0820211F,
		0xC78, 0x0810221F,
		0xC78, 0x0650231F,
		0xC78, 0x0640241F,
		0xC78, 0x0630251F,
		0xC78, 0x0450261F,
		0xC78, 0x0440271F,
		0xC78, 0x0430281F,
		0xC78, 0x0420291F,
		0xC78, 0x04102A1F,
		0xC78, 0x04002B1F,
		0xC78, 0x02402C1F,
		0xC78, 0x02302D1F,
		0xC78, 0x00602E1F,
		0xC78, 0x00502F1F,
		0xC78, 0x0040301F,
		0xC78, 0x0030311F,
		0xC78, 0x0020321F,
		0xC78, 0x0010331F,
		0xC78, 0x0000341F,
		0xC78, 0x0000351F,
		0xC78, 0x0000361F,
		0xC78, 0x0000371F,
		0xC78, 0x0000381F,
		0xC78, 0x0000391F,
		0xC78, 0x00003A1F,
		0xC78, 0x00003B1F,
		0xC78, 0x00003C1F,
		0xC78, 0x00003D1F,
		0xC78, 0x00003E1F,
		0xC78, 0x00003F1F,
	0x90001000,	0x00000000,	0x40000000,	0x00000000,
		0xC78, 0x0FD0001F,
		0xC78, 0x0FC0011F,
		0xC78, 0x0FB0021F,
		0xC78, 0x0FA0031F,
		0xC78, 0x0F90041F,
		0xC78, 0x0F80051F,
		0xC78, 0x0F70061F,
		0xC78, 0x0F60071F,
		0xC78, 0x0F50081F,
		0xC78, 0x0F40091F,
		0xC78, 0x0F300A1F,
		0xC78, 0x0F200B1F,
		0xC78, 0x0F100C1F,
		0xC78, 0x0F000D1F,
		0xC78, 0x0EF00E1F,
		0xC78, 0x0EE00F1F,
		0xC78, 0x0ED0101F,
		0xC78, 0x0EC0111F,
		0xC78, 0x0EB0121F,
		0xC78, 0x0EA0131F,
		0xC78, 0x0E90141F,
		0xC78, 0x0E80151F,
		0xC78, 0x0E70161F,
		0xC78, 0x0E60171F,
		0xC78, 0x0E50181F,
		0xC78, 0x0E40191F,
		0xC78, 0x0E301A1F,
		0xC78, 0x0E201B1F,
		0xC78, 0x0E101C1F,
		0xC78, 0x0E001D1F,
		0xC78, 0x0C401E1F,
		0xC78, 0x0C301F1F,
		0xC78, 0x0C20201F,
		0xC78, 0x0C10211F,
		0xC78, 0x0C00221F,
		0xC78, 0x0A40231F,
		0xC78, 0x0A30241F,
		0xC78, 0x0A20251F,
		0xC78, 0x0A10261F,
		0xC78, 0x0A00271F,
		0xC78, 0x0820281F,
		0xC78, 0x0810291F,
		0xC78, 0x08002A1F,
		0xC78, 0x06402B1F,
		0xC78, 0x06302C1F,
		0xC78, 0x06202D1F,
		0xC78, 0x06102E1F,
		0xC78, 0x06002F1F,
		0xC78, 0x0420301F,
		0xC78, 0x0410311F,
		0xC78, 0x0400321F,
		0xC78, 0x0060331F,
		0xC78, 0x0050341F,
		0xC78, 0x0040351F,
		0xC78, 0x0030361F,
		0xC78, 0x0020371F,
		0xC78, 0x0010381F,
		0xC78, 0x0000391F,
		0xC78, 0x00003A1F,
		0xC78, 0x00003B1F,
		0xC78, 0x00003C1F,
		0xC78, 0x00003D1F,
		0xC78, 0x00003E1F,
		0xC78, 0x00003F1F,
	0x90002000,	0x00000000,	0x40000000,	0x00000000,
		0xC78, 0x0FD0001F,
		0xC78, 0x0FC0011F,
		0xC78, 0x0FB0021F,
		0xC78, 0x0FA0031F,
		0xC78, 0x0F90041F,
		0xC78, 0x0F80051F,
		0xC78, 0x0F70061F,
		0xC78, 0x0F60071F,
		0xC78, 0x0F50081F,
		0xC78, 0x0F40091F,
		0xC78, 0x0F300A1F,
		0xC78, 0x0F200B1F,
		0xC78, 0x0F100C1F,
		0xC78, 0x0F000D1F,
		0xC78, 0x0EF00E1F,
		0xC78, 0x0EE00F1F,
		0xC78, 0x0ED0101F,
		0xC78, 0x0EC0111F,
		0xC78, 0x0EB0121F,
		0xC78, 0x0EA0131F,
		0xC78, 0x0E90141F,
		0xC78, 0x0E80151F,
		0xC78, 0x0E70161F,
		0xC78, 0x0CB0171F,
		0xC78, 0x0CA0181F,
		0xC78, 0x0C90191F,
		0xC78, 0x0C801A1F,
		0xC78, 0x0C701B1F,
		0xC78, 0x0C601C1F,
		0xC78, 0x0C501D1F,
		0xC78, 0x0C401E1F,
		0xC78, 0x0C301F1F,
		0xC78, 0x0C20201F,
		0xC78, 0x0C10211F,
		0xC78, 0x0C00221F,
		0xC78, 0x0A40231F,
		0xC78, 0x0A30241F,
		0xC78, 0x0A20251F,
		0xC78, 0x0840261F,
		0xC78, 0x0830271F,
		0xC78, 0x0820281F,
		0xC78, 0x0810291F,
		0xC78, 0x08002A1F,
		0xC78, 0x06402B1F,
		0xC78, 0x06302C1F,
		0xC78, 0x06202D1F,
		0xC78, 0x04402E1F,
		0xC78, 0x04302F1F,
		0xC78, 0x0420301F,
		0xC78, 0x0410311F,
		0xC78, 0x0400321F,
		0xC78, 0x0050331F,
		0xC78, 0x0040341F,
		0xC78, 0x0030351F,
		0xC78, 0x0020361F,
		0xC78, 0x0010371F,
		0xC78, 0x0000381F,
		0xC78, 0x0000391F,
		0xC78, 0x00003A1F,
		0xC78, 0x00003B1F,
		0xC78, 0x00003C1F,
		0xC78, 0x00003D1F,
		0xC78, 0x00003E1F,
		0xC78, 0x00003F1F,
	0xA0000000,	0x00000000,
		0xC78, 0x0FF0001F,
		0xC78, 0x0FE0011F,
		0xC78, 0x0FD0021F,
		0xC78, 0x0FC0031F,
		0xC78, 0x0FB0041F,
		0xC78, 0x0FA0051F,
		0xC78, 0x0F90061F,
		0xC78, 0x0F80071F,
		0xC78, 0x0F70081F,
		0xC78, 0x0F60091F,
		0xC78, 0x0F500A1F,
		0xC78, 0x0F400B1F,
		0xC78, 0x0F300C1F,
		0xC78, 0x0F200D1F,
		0xC78, 0x0F100E1F,
		0xC78, 0x0F000F1F,
		0xC78, 0x0EF0101F,
		0xC78, 0x0EE0111F,
		0xC78, 0x0ED0121F,
		0xC78, 0x0EC0131F,
		0xC78, 0x0EB0141F,
		0xC78, 0x0EA0151F,
		0xC78, 0x0E90161F,
		0xC78, 0x0E80171F,
		0xC78, 0x0E70181F,
		0xC78, 0x0E60191F,
		0xC78, 0x0E501A1F,
		0xC78, 0x0E401B1F,
		0xC78, 0x0E301C1F,
		0xC78, 0x0E201D1F,
		0xC78, 0x0E101E1F,
		0xC78, 0x0C401F1F,
		0xC78, 0x0C30201F,
		0xC78, 0x0C20211F,
		0xC78, 0x0C10221F,
		0xC78, 0x0C00231F,
		0xC78, 0x0A40241F,
		0xC78, 0x0A30251F,
		0xC78, 0x0A20261F,
		0xC78, 0x0A10271F,
		0xC78, 0x0A00281F,
		0xC78, 0x0820291F,
		0xC78, 0x08102A1F,
		0xC78, 0x08002B1F,
		0xC78, 0x06402C1F,
		0xC78, 0x06302D1F,
		0xC78, 0x06202E1F,
		0xC78, 0x06102F1F,
		0xC78, 0x0420301F,
		0xC78, 0x0410311F,
		0xC78, 0x0400321F,
		0xC78, 0x0240331F,
		0xC78, 0x0230341F,
		0xC78, 0x0220351F,
		0xC78, 0x0210361F,
		0xC78, 0x0200371F,
		0xC78, 0x0020381F,
		0xC78, 0x0010391F,
		0xC78, 0x00003A1F,
		0xC78, 0x00003B1F,
		0xC78, 0x00003C1F,
		0xC78, 0x00003D1F,
		0xC78, 0x00003E1F,
		0xC78, 0x00003F1F,
	0xB0000000,	0x00000000,
	0x80001003,	0x00000000,	0x40000000,	0x00000000,
		0xC78, 0x0DC0401F,
		0xC78, 0x0DB0411F,
		0xC78, 0x0DA0421F,
		0xC78, 0x0D90431F,
		0xC78, 0x0D80441F,
		0xC78, 0x0D70451F,
		0xC78, 0x0D60461F,
		0xC78, 0x0D50471F,
		0xC78, 0x0D40481F,
		0xC78, 0x0D30491F,
		0xC78, 0x0D204A1F,
		0xC78, 0x0D104B1F,
		0xC78, 0x0D004C1F,
		0xC78, 0x0CF04D1F,
		0xC78, 0x0CE04E1F,
		0xC78, 0x0CD04F1F,
		0xC78, 0x0B20501F,
		0xC78, 0x0930511F,
		0xC78, 0x0920521F,
		0xC78, 0x0910531F,
		0xC78, 0x0900541F,
		0xC78, 0x08F0551F,
		0xC78, 0x08E0561F,
		0xC78, 0x08D0571F,
		0xC78, 0x08C0581F,
		0xC78, 0x08B0591F,
		0xC78, 0x08A05A1F,
		0xC78, 0x08905B1F,
		0xC78, 0x08805C1F,
		0xC78, 0x08705D1F,
		0xC78, 0x08605E1F,
		0xC78, 0x08505F1F,
		0xC78, 0x0840601F,
		0xC78, 0x0830611F,
		0xC78, 0x0820621F,
		0xC78, 0x0810631F,
		0xC78, 0x0650641F,
		0xC78, 0x0640651F,
		0xC78, 0x0630661F,
		0xC78, 0x0450671F,
		0xC78, 0x0440681F,
		0xC78, 0x0430691F,
		0xC78, 0x04206A1F,
		0xC78, 0x04106B1F,
		0xC78, 0x04006C1F,
		0xC78, 0x02406D1F,
		0xC78, 0x02306E1F,
		0xC78, 0x00606F1F,
		0xC78, 0x0050701F,
		0xC78, 0x0040711F,
		0xC78, 0x0030721F,
		0xC78, 0x0020731F,
		0xC78, 0x0010741F,
		0xC78, 0x0000751F,
		0xC78, 0x0000761F,
		0xC78, 0x0000771F,
		0xC78, 0x0000781F,
		0xC78, 0x0000791F,
		0xC78, 0x00007A1F,
		0xC78, 0x00007B1F,
		0xC78, 0x00007C1F,
		0xC78, 0x00007D1F,
		0xC78, 0x00007E1F,
		0xC78, 0x00007F1F,
	0x90001000,	0x00000000,	0x40000000,	0x00000000,
		0xC78, 0x0FF0401F,
		0xC78, 0x0FE0411F,
		0xC78, 0x0FD0421F,
		0xC78, 0x0FC0431F,
		0xC78, 0x0FB0441F,
		0xC78, 0x0FA0451F,
		0xC78, 0x0F90461F,
		0xC78, 0x0F80471F,
		0xC78, 0x0F70481F,
		0xC78, 0x0F60491F,
		0xC78, 0x0F504A1F,
		0xC78, 0x0F404B1F,
		0xC78, 0x0F304C1F,
		0xC78, 0x0F204D1F,
		0xC78, 0x0F104E1F,
		0xC78, 0x0F004F1F,
		0xC78, 0x0EF0501F,
		0xC78, 0x0EE0511F,
		0xC78, 0x0B70521F,
		0xC78, 0x0B60531F,
		0xC78, 0x0B50541F,
		0xC78, 0x0B40551F,
		0xC78, 0x0B30561F,
		0xC78, 0x0B20571F,
		0xC78, 0x0B10581F,
		0xC78, 0x0B00591F,
		0xC78, 0x0AF05A1F,
		0xC78, 0x0AE05B1F,
		0xC78, 0x0AD05C1F,
		0xC78, 0x0AC05D1F,
		0xC78, 0x0AB05E1F,
		0xC78, 0x0AA05F1F,
		0xC78, 0x0A90601F,
		0xC78, 0x0A80611F,
		0xC78, 0x0A70621F,
		0xC78, 0x0A60631F,
		0xC78, 0x0A50641F,
		0xC78, 0x0A40651F,
		0xC78, 0x0A30661F,
		0xC78, 0x0A20671F,
		0xC78, 0x0A10681F,
		0xC78, 0x0A00691F,
		0xC78, 0x08206A1F,
		0xC78, 0x08106B1F,
		0xC78, 0x08006C1F,
		0xC78, 0x06406D1F,
		0xC78, 0x06306E1F,
		0xC78, 0x06206F1F,
		0xC78, 0x0610701F,
		0xC78, 0x0600711F,
		0xC78, 0x0420721F,
		0xC78, 0x0410731F,
		0xC78, 0x0400741F,
		0xC78, 0x0060751F,
		0xC78, 0x0050761F,
		0xC78, 0x0040771F,
		0xC78, 0x0030781F,
		0xC78, 0x0020791F,
		0xC78, 0x00107A1F,
		0xC78, 0x00007B1F,
		0xC78, 0x00007C1F,
		0xC78, 0x00007D1F,
		0xC78, 0x00007E1F,
		0xC78, 0x00007F1F,
	0x90002000,	0x00000000,	0x40000000,	0x00000000,
		0xC78, 0x0FF0401F,
		0xC78, 0x0FE0411F,
		0xC78, 0x0FD0421F,
		0xC78, 0x0FC0431F,
		0xC78, 0x0FB0441F,
		0xC78, 0x0FA0451F,
		0xC78, 0x0F90461F,
		0xC78, 0x0F80471F,
		0xC78, 0x0F70481F,
		0xC78, 0x0F60491F,
		0xC78, 0x0F504A1F,
		0xC78, 0x0F404B1F,
		0xC78, 0x0F304C1F,
		0xC78, 0x0F204D1F,
		0xC78, 0x0F104E1F,
		0xC78, 0x0F004F1F,
		0xC78, 0x0EF0501F,
		0xC78, 0x0EE0511F,
		0xC78, 0x0B70521F,
		0xC78, 0x0B60531F,
		0xC78, 0x0B50541F,
		0xC78, 0x0B40551F,
		0xC78, 0x0B30561F,
		0xC78, 0x0B20571F,
		0xC78, 0x0B10581F,
		0xC78, 0x0B00591F,
		0xC78, 0x0AF05A1F,
		0xC78, 0x0AE05B1F,
		0xC78, 0x0AD05C1F,
		0xC78, 0x0AC05D1F,
		0xC78, 0x0AB05E1F,
		0xC78, 0x0AA05F1F,
		0xC78, 0x0A90601F,
		0xC78, 0x0A80611F,
		0xC78, 0x0A70621F,
		0xC78, 0x0A60631F,
		0xC78, 0x0A50641F,
		0xC78, 0x0A40651F,
		0xC78, 0x0A30661F,
		0xC78, 0x0A20671F,
		0xC78, 0x0A10681F,
		0xC78, 0x0A00691F,
		0xC78, 0x08206A1F,
		0xC78, 0x08106B1F,
		0xC78, 0x08006C1F,
		0xC78, 0x06406D1F,
		0xC78, 0x06306E1F,
		0xC78, 0x06206F1F,
		0xC78, 0x0610701F,
		0xC78, 0x0600711F,
		0xC78, 0x0420721F,
		0xC78, 0x0410731F,
		0xC78, 0x0400741F,
		0xC78, 0x0060751F,
		0xC78, 0x0050761F,
		0xC78, 0x0040771F,
		0xC78, 0x0030781F,
		0xC78, 0x0020791F,
		0xC78, 0x00107A1F,
		0xC78, 0x00007B1F,
		0xC78, 0x00007C1F,
		0xC78, 0x00007D1F,
		0xC78, 0x00007E1F,
		0xC78, 0x00007F1F,
	0xA0000000,	0x00000000,
		0xC78, 0x0FF0401F,
		0xC78, 0x0FE0411F,
		0xC78, 0x0FD0421F,
		0xC78, 0x0FC0431F,
		0xC78, 0x0FB0441F,
		0xC78, 0x0FA0451F,
		0xC78, 0x0F90461F,
		0xC78, 0x0F80471F,
		0xC78, 0x0F70481F,
		0xC78, 0x0F60491F,
		0xC78, 0x0F504A1F,
		0xC78, 0x0F404B1F,
		0xC78, 0x0F304C1F,
		0xC78, 0x0F204D1F,
		0xC78, 0x0F104E1F,
		0xC78, 0x0F004F1F,
		0xC78, 0x0EF0501F,
		0xC78, 0x0EE0511F,
		0xC78, 0x0B70521F,
		0xC78, 0x0B60531F,
		0xC78, 0x0B50541F,
		0xC78, 0x0B40551F,
		0xC78, 0x0B30561F,
		0xC78, 0x0B20571F,
		0xC78, 0x0B10581F,
		0xC78, 0x0B00591F,
		0xC78, 0x0AF05A1F,
		0xC78, 0x0AE05B1F,
		0xC78, 0x0AD05C1F,
		0xC78, 0x0AC05D1F,
		0xC78, 0x0AB05E1F,
		0xC78, 0x0AA05F1F,
		0xC78, 0x0A90601F,
		0xC78, 0x0A80611F,
		0xC78, 0x0A70621F,
		0xC78, 0x0A60631F,
		0xC78, 0x0A50641F,
		0xC78, 0x0A40651F,
		0xC78, 0x0A30661F,
		0xC78, 0x0A20671F,
		0xC78, 0x0A10681F,
		0xC78, 0x0A00691F,
		0xC78, 0x08206A1F,
		0xC78, 0x08106B1F,
		0xC78, 0x08006C1F,
		0xC78, 0x06406D1F,
		0xC78, 0x06306E1F,
		0xC78, 0x06206F1F,
		0xC78, 0x0610701F,
		0xC78, 0x0600711F,
		0xC78, 0x0420721F,
		0xC78, 0x0410731F,
		0xC78, 0x0400741F,
		0xC78, 0x0060751F,
		0xC78, 0x0050761F,
		0xC78, 0x0040771F,
		0xC78, 0x0030781F,
		0xC78, 0x0020791F,
		0xC78, 0x00107A1F,
		0xC78, 0x00007B1F,
		0xC78, 0x00007C1F,
		0xC78, 0x00007D1F,
		0xC78, 0x00007E1F,
		0xC78, 0x00007F1F,
	0xB0000000,	0x00000000,
	0x80001003,	0x00000000,	0x40000000,	0x00000000,
		0xC50, 0x00340222,
		0xC50, 0x00340220,
	0x90001000,	0x00000000,	0x40000000,	0x00000000,
		0xC50, 0x00340022,
		0xC50, 0x00340020,
	0xA0000000,	0x00000000,
		0xC50, 0x00340022,
		0xC50, 0x00340020,
	0xB0000000,	0x00000000,
	0x80001003,	0x00000000,	0x40000000,	0x00000000,
		0xC58, 0x00000222,
		0xC58, 0x00000220,
	0xA0000000,	0x00000000,
		0xC58, 0x00000022,
		0xC58, 0x00000020,
	0xB0000000,	0x00000000,

};

void
ODM_ReadAndConfig_MP_8197F_AGC_TAB(
	IN   PDM_ODM_T  pDM_Odm
)
{
	u4Byte     i         = 0;
	u1Byte     cCond;
	BOOLEAN bMatched = TRUE, bSkipped = FALSE;
	u4Byte     ArrayLen    = sizeof(Array_MP_8197F_AGC_TAB)/sizeof(u4Byte);
	pu4Byte    Array       = Array_MP_8197F_AGC_TAB;
	
	u4Byte	v1 = 0, v2 = 0, pre_v1 = 0, pre_v2 = 0;

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, ("===> ODM_ReadAndConfig_MP_8197F_AGC_TAB\n"));

	while ((i + 1) < ArrayLen) {
		v1 = Array[i];
		v2 = Array[i + 1];

		if (v1 & (BIT31 | BIT30)) {/*positive & negative condition*/
			if (v1 & BIT31) {/* positive condition*/
				cCond  = (u1Byte)((v1 & (BIT29|BIT28)) >> 28);
				if (cCond == COND_ENDIF) {/*end*/
					bMatched = TRUE;
					bSkipped = FALSE;
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, ("ENDIF\n"));
				} else if (cCond == COND_ELSE) { /*else*/
					bMatched = bSkipped?FALSE:TRUE;
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, ("ELSE\n"));
				} else {/*if , else if*/
					pre_v1 = v1;
					pre_v2 = v2;
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, ("IF or ELSE IF\n"));
				}
			} else if (v1 & BIT30) { /*negative condition*/
				if (bSkipped == FALSE) {
					if (CheckPositive(pDM_Odm, pre_v1, pre_v2, v1, v2)) {
						bMatched = TRUE;
						bSkipped = TRUE;
					} else {
						bMatched = FALSE;
						bSkipped = FALSE;
					}
				} else
					bMatched = FALSE;
			}
		} else {
			if (bMatched)
				odm_ConfigBB_AGC_8197F(pDM_Odm, v1, bMaskDWord, v2);
		}
		i = i + 2;
	}
}

u4Byte
ODM_GetVersion_MP_8197F_AGC_TAB(void)
{
	   return 37;
}

/******************************************************************************
*                           PHY_REG.TXT
******************************************************************************/

u4Byte Array_MP_8197F_PHY_REG[] = { 
		0x800, 0x00005308,
		0x804, 0x00004001,
		0x808, 0x0000FC00,
		0x80C, 0x00000000,
		0x810, 0x20200022,
		0x814, 0x020C3D10,
		0x818, 0x000003C5,
		0x81C, 0x07000000,
		0x820, 0x01000100,
		0x824, 0x00390204,
		0x828, 0x01000100,
		0x82C, 0x00390204,
		0x830, 0x00000000,
		0x834, 0x00690200,
		0x838, 0x00000004,
		0x83C, 0x00690200,
		0x840, 0x00010000,
		0x844, 0x00010000,
		0x848, 0x00000000,
		0x84C, 0x00000000,
		0x850, 0x00031FE0,
		0x854, 0x00000000,
		0x858, 0x569A569A,
		0x85C, 0x00400040,
		0x860, 0x66F60000,
		0x864, 0x061F0000,
		0x868, 0x0F7F0130,
		0x86C, 0x0F7F0130,
		0x870, 0x00000300,
		0x874, 0x04003400,
		0x878, 0x08080808,
		0x87C, 0x004F0201,
		0x880, 0xD8001402,
		0x884, 0xC0000120,
		0x888, 0x00000000,
		0x88C, 0xCC0000C0,
		0x890, 0x00000000,
		0x894, 0xFFFFFFFE,
		0x898, 0x40302010,
		0x89C, 0x00706050,
		0x8A0, 0x00000000,
		0x8A4, 0x00000000,
		0x8A8, 0x00000109,
		0x8AC, 0x00000000,
		0x8B0, 0x00000000,
		0x8B4, 0x00000000,
		0x8B8, 0x00000000,
		0x8BC, 0x00000000,
		0x8C0, 0x03910C40,
		0x8C4, 0x00000000,
		0x8C8, 0x00000000,
		0x8CC, 0x00000000,
		0x8D0, 0x00000000,
		0x8D4, 0x00000000,
		0x8D8, 0x00000000,
		0x8DC, 0x00000000,
		0x8E0, 0x02000200,
		0x8E4, 0x00000000,
		0x8E8, 0xD8080000,
		0x8EC, 0x00000000,
		0x900, 0x00000000,
		0x904, 0x00000023,
		0x908, 0x00000F00,
		0x90C, 0x81121313,
		0x910, 0x024C0000,
		0x914, 0x00000000,
		0x918, 0x00000000,
		0x91C, 0x00000000,
		0x920, 0x00000000,
		0x924, 0x00000000,
		0x928, 0x00000000,
		0x92C, 0x00010040,
	0x80001003,	0x00000000,	0x40000000,	0x00000000,
		0x930, 0xFFFF02FF,
		0x934, 0xFFFFFFF5,
		0x938, 0xFFFFFFFF,
		0x93C, 0xFFFF502F,
		0x940, 0xFF54FF0F,
		0x944, 0x3F3F0F0C,
	0xA0000000,	0x00000000,
		0x930, 0x88000000,
		0x934, 0x00000245,
		0x938, 0x00024588,
		0x93C, 0x00000000,
		0x940, 0x000007FF,
		0x944, 0x3F3F0000,
	0xB0000000,	0x00000000,
		0x948, 0x000001A3,
		0x94C, 0x20200008,
	0x80001003,	0x00000000,	0x40000000,	0x00000000,
		0x950, 0x00338097,
	0x90002000,	0x00000000,	0x40000000,	0x00000000,
		0x950, 0x00338397,
	0xA0000000,	0x00000000,
		0x950, 0x00338497,
	0xB0000000,	0x00000000,
		0x954, 0x00000000,
	0x80001003,	0x00000000,	0x40000000,	0x00000000,
		0x958, 0x4BC2D87A,
	0xA0000000,	0x00000000,
		0x958, 0x4BC5D87A,
	0xB0000000,	0x00000000,
		0x95C, 0x06EB9B79,
		0x960, 0x00000000,
		0x964, 0x00000000,
		0x968, 0x00000000,
		0x96C, 0x00000003,
		0x970, 0x00000000,
		0x974, 0x00000000,
		0x978, 0x00000000,
		0x97C, 0x10030000,
		0x980, 0x00000000,
		0x984, 0x00050050,
		0x988, 0x000A5704,
		0x98C, 0x1461C826,
		0x990, 0x0001469E,
		0x994, 0x008858D1,
		0x998, 0x700494C9,
		0x99C, 0x77777777,
		0x9A0, 0x00000000,
		0x9A4, 0x00000000,
		0x9A8, 0x00000000,
		0x9AC, 0x00020000,
		0x9B0, 0x00000000,
		0x9B4, 0x00000000,
		0x9B8, 0x00000000,
		0x9BC, 0x00000000,
		0x9C0, 0x00000011,
		0x9C4, 0x00000000,
		0x9C8, 0x00000000,
		0x9CC, 0x00000000,
		0x9D0, 0x00000000,
		0x9D4, 0x00000000,
		0x9D8, 0x00000000,
		0x9DC, 0x00000000,
		0x9E0, 0x00000000,
		0x9E4, 0x00000000,
		0x9E8, 0x00000000,
		0x9EC, 0x00000000,
		0x9F0, 0x80000000,
		0x9F4, 0x00000000,
		0x9F8, 0x00000000,
		0xA00, 0x00D047C8,
		0xA04, 0x80FF800C,
		0xA08, 0x9C838300,
		0xA0C, 0x2E20200F,
		0xA10, 0x9500BB78,
		0xA14, 0x1114D028,
		0xA18, 0x00881117,
		0xA1C, 0x89140F00,
		0xA20, 0xE82C0001,
		0xA24, 0x64B80C1C,
		0xA28, 0x00008810,
		0xA2C, 0x00D20000,
		0xA70, 0x101FBF00,
		0xA74, 0x00000107,
		0xA78, 0x00008900,
		0xA7C, 0x225B0606,
		0xA80, 0x21807532,
		0xA84, 0x00200000,
		0xA88, 0x040C0000,
		0xA8C, 0x12345678,
		0xA90, 0xABCDEF00,
		0xA94, 0x001B1B89,
		0xA98, 0x00000000,
		0xA9C, 0x00060000,
		0xAA0, 0x00000000,
		0xAA4, 0x00010000,
	0x80001003,	0x00000000,	0x40000000,	0x00000000,
		0xAA8, 0xEAEF0004,
	0xA0000000,	0x00000000,
		0xAA8, 0xEAEF0004,
	0xB0000000,	0x00000000,
		0xAAC, 0x01235667,
		0xAB0, 0x00000000,
		0xAB4, 0x00201402,
		0xB00, 0x00000000,
		0xB04, 0x00000000,
		0xB08, 0x00000000,
		0xB0C, 0x00000000,
		0xB10, 0x00000000,
		0xB14, 0x00000000,
		0xB18, 0x00000000,
		0xB1C, 0x00000000,
		0xB20, 0x00000000,
		0xB24, 0x00000000,
		0xB28, 0x00000000,
		0xB2C, 0x00000000,
		0xB30, 0x00000000,
		0xB34, 0x00000000,
		0xB38, 0x00000000,
		0xB3C, 0x00000000,
		0xB40, 0x00000000,
		0xB44, 0x00000000,
		0xB48, 0x00000000,
		0xB4C, 0x00000000,
		0xB50, 0x00000000,
		0xB54, 0x00000000,
		0xB58, 0x00000000,
		0xB5C, 0x00000000,
		0xB60, 0x00000000,
		0xB64, 0x00000000,
		0xB68, 0x80000000,
		0xB6C, 0x80000000,
		0xB70, 0x00000000,
		0xB74, 0x00000000,
		0xB78, 0x00000000,
		0xB7C, 0x00000000,
		0xB80, 0x00000000,
		0xB84, 0x00000000,
		0xB88, 0x00000000,
		0xB8C, 0x00000000,
		0xB90, 0x00000000,
		0xB94, 0x00000000,
		0xB98, 0x00000000,
		0xB9C, 0x00000000,
		0xBA0, 0x00000000,
		0xBA4, 0x00000000,
		0xBA8, 0x00000000,
		0xBAC, 0x00000000,
		0xBB0, 0x00440000,
		0xBB4, 0x00000000,
		0xBB8, 0x00000000,
		0xBBC, 0x00000000,
		0xBD0, 0x00000000,
		0xBD4, 0x00000000,
		0xBDC, 0x02000000,
		0xBE0, 0x02000000,
		0xBE4, 0x02000000,
		0xBE8, 0x02000000,
		0xBEC, 0x02000000,
		0xBF0, 0x02000000,
		0xBF4, 0x02000000,
		0xBF8, 0x02000000,
		0xC00, 0x00000080,
		0xC04, 0x6F005433,
		0xC08, 0x000004E4,
		0xC0C, 0x6C6C6C6C,
		0xC10, 0x20000000,
		0xC14, 0x40000100,
		0xC18, 0x20000000,
		0xC1C, 0x40000100,
		0xC20, 0x00000000,
		0xC24, 0x40000100,
		0xC28, 0x00000000,
		0xC2C, 0x40000100,
		0xC30, 0x0401A807,
		0xC34, 0x30000040,
		0xC38, 0x23408080,
	0x80001003,	0x00000000,	0x40000000,	0x00000000,
		0xC3C, 0x00002E04,
	0xA0000000,	0x00000000,
		0xC3C, 0x00002F0C,
	0xB0000000,	0x00000000,
		0xC40, 0x1CF8403F,
		0xC44, 0x000100C7,
		0xC48, 0xEC060106,
		0xC4C, 0x00F203FD,
		0xC50, 0x00340220,
		0xC54, 0x04008017,
		0xC58, 0x00000020,
		0xC5C, 0x00708492,
		0xC60, 0x002E0200,
		0xC64, 0x5012838B,
		0xC68, 0x47C006C7,
		0xC6C, 0x00000035,
		0xC70, 0x00000007,
		0xC74, 0x020251A9,
		0xC78, 0x0FE07F1F,
		0xC7C, 0x00B91612,
	0x80001003,	0x00000000,	0x40000000,	0x00000000,
		0xC80, 0x24000090,
	0xA0000000,	0x00000000,
		0xC80, 0x40000100,
	0xB0000000,	0x00000000,
		0xC84, 0x12000000,
	0x80001003,	0x00000000,	0x40000000,	0x00000000,
		0xC88, 0x24000090,
	0xA0000000,	0x00000000,
		0xC88, 0x40000100,
	0xB0000000,	0x00000000,
		0xC8C, 0xA0240000,
		0xC90, 0x40000100,
		0xC94, 0x00000000,
		0xC98, 0x40000100,
		0xC9C, 0x0000BDC8,
		0xCA0, 0x00000000,
		0xCA4, 0x098300A0,
		0xCA8, 0x00000000,
		0xCAC, 0x00F57B1A,
		0xCB0, 0x00000000,
		0xCB4, 0x00000000,
		0xCB8, 0x00000000,
		0xCBC, 0x28100200,
		0xCC0, 0x0010A3D0,
		0xCC4, 0x00000F7D,
		0xCC8, 0x00000000,
		0xCCC, 0x00000000,
		0xCD0, 0x00000000,
		0xCD4, 0x00000000,
		0xCD8, 0x64B22427,
		0xCDC, 0x00766932,
		0xCE0, 0x40201002,
		0xCE4, 0x00000000,
		0xCE8, 0x40E04407,
		0xCEC, 0x2E572000,
		0xCF0, 0x00000000,
		0xCF4, 0x00000000,
		0xCF8, 0x00000000,
		0xD00, 0x000B0680,
		0xD04, 0xC0000403,
		0xD08, 0x0000907F,
		0xD0C, 0x00000201,
		0xD10, 0x06288888,
		0xD14, 0x8888366B,
		0xD18, 0x7D806DB3,
		0xD1C, 0x0000007F,
		0xD20, 0x167401B8,
		0xD24, 0x0000018B,
		0xD28, 0xD513FF7D,
		0xD2C, 0xCC979975,
		0xD30, 0x00008000,
		0xD34, 0x40608000,
		0xD38, 0xA8000000,
		0xD3C, 0x00026EE2,
		0xD40, 0x00000000,
		0xD44, 0x00000000,
		0xD48, 0x00000000,
		0xD4C, 0x00000000,
		0xD50, 0x6437140A,
		0xD54, 0x20500000,
		0xD58, 0x16161616,
		0xD5C, 0x70320064,
		0xD60, 0x4653DE60,
		0xD64, 0x7E718A3C,
		0xD68, 0x00000103,
		0xD6C, 0x00000000,
		0xD70, 0x00000000,
		0xD74, 0x00000000,
		0xD78, 0x00000000,
		0xD7C, 0x00000000,
		0xD80, 0x00000000,
		0xD84, 0x31310000,
		0xD88, 0xF0B50000,
		0xD8C, 0x00000000,
		0xD90, 0x00000000,
		0xD94, 0x44BBBB44,
		0xD98, 0x44BB44FF,
		0xD9C, 0x06033688,
		0xDA0, 0x00000002,
		0xDA4, 0x00000000,
		0xDA8, 0x00000000,
		0xDAC, 0x00000000,
		0xDB0, 0x00000000,
		0xDB4, 0x00000000,
		0xDB8, 0x00000000,
		0xDBC, 0x00000000,
		0xDC0, 0x00000000,
		0xDC4, 0x00210000,
		0xDC8, 0x00000200,
		0xDCC, 0x0000000F,
		0xDD0, 0x00000020,
		0xDD4, 0x00007F7F,
		0xDD8, 0xE5AF1854,
		0xDDC, 0x000040AB,
		0xDE0, 0x00000000,
		0xDE4, 0x00000000,
		0xDE8, 0x0002030B,
		0xDEC, 0x0000A987,
		0xDF0, 0x00005807,
		0xDF4, 0x00000000,
		0xDF8, 0x00000005,
		0xE00, 0x00000000,
		0xE04, 0x00000000,
		0xE08, 0x00000000,
		0xE0C, 0x000000FF,
		0xE10, 0x00000000,
		0xE14, 0x00000000,
		0xE18, 0x00000000,
		0xE1C, 0x00000000,
		0xE20, 0x00000000,
		0xE24, 0x00200000,
		0xE28, 0x00000000,
		0xE2C, 0x00000000,
		0xE30, 0x01007C00,
		0xE34, 0x01004800,
		0xE38, 0x10008C0F,
		0xE3C, 0x3C008C0F,
		0xE40, 0x01007C00,
		0xE44, 0x00000000,
		0xE48, 0x00000000,
		0xE4C, 0x00000000,
		0xE50, 0x01007C00,
		0xE54, 0x01004800,
		0xE58, 0x10008C0F,
		0xE5C, 0x3C008C0F,
		0xE60, 0x02100000,
		0xE64, 0xBBBBBBBB,
		0xE68, 0x40404040,
		0xE6C, 0x80408040,
		0xE70, 0x80408040,
		0xE74, 0x40404040,
		0xE78, 0x00400040,
		0xE7C, 0x40404040,
		0xE80, 0x00FF0000,
		0xE84, 0x80408040,
		0xE88, 0x40404040,
		0xE8C, 0x80408040,
		0xE90, 0x00000000,
		0xE94, 0x01000000,
		0xE98, 0x00000000,
		0xE9C, 0x00000000,
		0xEA0, 0x00000000,
		0xEA4, 0x01000000,
		0xEA8, 0x00000000,
		0xEAC, 0x00000000,
		0xEB0, 0x00000000,
		0xEB4, 0x01000000,
		0xEB8, 0x00000000,
		0xEBC, 0x00000000,
		0xEC0, 0x00000000,
		0xEC4, 0x01000000,
		0xEC8, 0x00000000,
		0xECC, 0x00000000,
		0xED0, 0x80408040,
		0xED4, 0x80408040,
		0xED8, 0x80408040,
		0xEDC, 0xC040C040,
		0xEE0, 0xC040C040,
		0xEE4, 0x00400040,
		0xEE8, 0xD8001402,
		0xEEC, 0xC0000120,
		0xEF0, 0x02000B09,
		0xEF4, 0x00000001,
		0xEF8, 0x00000000,
		0xF00, 0x00000300,
		0xF04, 0x00000000,
		0xF08, 0x0002030B,
		0xF0C, 0x0000A987,
		0xF10, 0x00005807,
		0xF14, 0x00000003,
		0xF18, 0x00020002,
		0xF1C, 0x8000001F,
		0xF20, 0x00000000,
		0xF24, 0x00000000,
		0xF28, 0x00000000,
		0xF2C, 0x00000000,
		0xF30, 0x00000000,
		0xF34, 0x00000000,
		0xF38, 0x00030155,
		0xF3C, 0x0000003A,
		0xF40, 0x00000000,
		0xF44, 0x00000000,
		0xF48, 0x00030000,
		0xF4C, 0x13000000,
		0xF50, 0x00000010,
		0xF54, 0x00000000,
		0xF58, 0x00000000,
		0xF5C, 0x00000000,
		0xF60, 0x00000000,
		0xF64, 0x00000000,
		0xF68, 0x00000000,
		0xF6C, 0x00000000,
		0xF70, 0x00000000,
		0xF74, 0x00000000,
		0xF78, 0x00000000,
		0xF7C, 0x00000000,
		0xF80, 0x00000000,
		0xF84, 0x00000000,
		0xF88, 0x00000000,
		0xF8C, 0x00000000,
		0xF90, 0x00000002,
		0xF94, 0x00000000,
		0xF98, 0x00000000,
		0xF9C, 0x00000000,
		0xFA0, 0x00000000,
		0xFA4, 0x0002030B,
		0xFA8, 0x0000A987,
		0xFAC, 0x00005807,
		0xFB0, 0x00000000,
		0xFB4, 0x00000000,
		0xFB8, 0x00000000,
		0xFBC, 0x00000000,
		0xFC0, 0x00C02001,
		0xFC4, 0x90063000,
		0xFC8, 0x00000018,
		0xFCC, 0x10003005,
		0xFD0, 0x0000E5AF,
		0xFD4, 0x00000000,
		0xFD8, 0x00000000,
		0xFDC, 0x00000000,
		0xFE0, 0x00000000,
		0xFE4, 0x00000000,
		0xFE8, 0x00000000,
		0xFEC, 0x00000000,
		0xFF0, 0x00000000,
		0xFF4, 0x00000000,
		0xFF8, 0x00000000,

};

void
ODM_ReadAndConfig_MP_8197F_PHY_REG(
	IN   PDM_ODM_T  pDM_Odm
)
{
	u4Byte     i         = 0;
	u1Byte     cCond;
	BOOLEAN bMatched = TRUE, bSkipped = FALSE;
	u4Byte     ArrayLen    = sizeof(Array_MP_8197F_PHY_REG)/sizeof(u4Byte);
	pu4Byte    Array       = Array_MP_8197F_PHY_REG;
	
	u4Byte	v1 = 0, v2 = 0, pre_v1 = 0, pre_v2 = 0;

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, ("===> ODM_ReadAndConfig_MP_8197F_PHY_REG\n"));

	while ((i + 1) < ArrayLen) {
		v1 = Array[i];
		v2 = Array[i + 1];

		if (v1 & (BIT31 | BIT30)) {/*positive & negative condition*/
			if (v1 & BIT31) {/* positive condition*/
				cCond  = (u1Byte)((v1 & (BIT29|BIT28)) >> 28);
				if (cCond == COND_ENDIF) {/*end*/
					bMatched = TRUE;
					bSkipped = FALSE;
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, ("ENDIF\n"));
				} else if (cCond == COND_ELSE) { /*else*/
					bMatched = bSkipped?FALSE:TRUE;
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, ("ELSE\n"));
				} else {/*if , else if*/
					pre_v1 = v1;
					pre_v2 = v2;
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, ("IF or ELSE IF\n"));
				}
			} else if (v1 & BIT30) { /*negative condition*/
				if (bSkipped == FALSE) {
					if (CheckPositive(pDM_Odm, pre_v1, pre_v2, v1, v2)) {
						bMatched = TRUE;
						bSkipped = TRUE;
					} else {
						bMatched = FALSE;
						bSkipped = FALSE;
					}
				} else
					bMatched = FALSE;
			}
		} else {
			if (bMatched)
				odm_ConfigBB_PHY_8197F(pDM_Odm, v1, bMaskDWord, v2);
		}
		i = i + 2;
	}
}

u4Byte
ODM_GetVersion_MP_8197F_PHY_REG(void)
{
	   return 37;
}

#endif /* end of HWIMG_SUPPORT*/

