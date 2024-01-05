#pragma once

#include "eval.h"

constexpr array<u64, 1000> stage_starts = {
        0x223e014300210000ull,
        0x000000000340e122ull,
        0x000000100323013eull,
        0x000000100033134eull,
        0x000000000310014eull,
        0x013e001300030002ull,
        0x123e243000000000ull,
        0x000e012400120001ull,
        0xe000412021002000ull,
        0x000000000142023eull,
        0x134e003200200000ull,
        0x100030004210e000ull,
        0xe200431031000000ull,
        0xe300232042102000ull,
        0x000e022400020001ull,
        0x010002000110e332ull,
        0x231e013300110000ull,
        0x000000003400e321ull,
        0x200021003510e000ull,
        0xe000421022001000ull,
        0x000000220432002eull,
        0x000100140123001eull,
        0x000000120005001eull,
        0x134e026203410000ull,
        0x034e022200000000ull,
        0xe421020201000000ull,
        0x100031004210e100ull,
        0x100040003121e100ull,
        0x000001102420e142ull,
        0xe421003000000000ull,
        0x002e003200230012ull,
        0xe300351012100000ull,
        0x000000000130124eull,
        0xe422031000000000ull,
        0x001e021400120001ull,
        0xe000140012002000ull,
        0xe521100200000000ull,
        0x100021204210e000ull,
        0x000100020043002eull,
        0xe310340021001000ull,
        0x000001000231034eull,
        0x000000120124001eull,
        0x210040003300e000ull,
        0xe430130002000000ull,
        0x012e014100030021ull,
        0x100020004210e210ull,
        0x022e014300210000ull,
        0x034e012202000000ull,
        0x023e014200010000ull,
        0x000202310242003eull,
        0x000100230015002eull,
        0x200014201300e000ull,
        0x004e013200120001ull,
        0x100030004320e100ull,
        0x001000310042131eull,
        0x000300040141012eull,
        0x000000210432000eull,
        0x000010003210e421ull,
        0x000000120052012eull,
        0xe421122000000000ull,
        0x000000300121123eull,
        0x000002001210e421ull,
        0xe300241220001000ull,
        0x000100030024000eull,
        0x000100030004002eull,
        0xe100400012101000ull,
        0x012e003100040012ull,
        0xe442120000000000ull,
        0xe000421021001200ull,
        0x200031003120e000ull,
        0x001e001400230000ull,
        0x100021301430e000ull,
        0x000202120124000eull,
        0x034e012000200000ull,
        0x000001000220e421ull,
        0x1000300042101e30ull,
        0x000000001020e431ull,
        0x215e002100100000ull,
        0x000000000002025eull,
        0x000000011003233eull,
        0xe300231040000000ull,
        0x000000004420e211ull,
        0x000100020124001eull,
        0x200010003410e210ull,
        0x000001002410e241ull,
        0x000100030124002eull,
        0x0001000e01340023ull,
        0x000000120142012eull,
        0x134e003000100000ull,
        0xe331240011000000ull,
        0x000100120124002eull,
        0x100020003210e400ull,
        0xe000243021001000ull,
        0xe322240002000100ull,
        0x024e003200010000ull,
        0x10004000e1003310ull,
        0x000000000020134eull,
        0x001e000500120001ull,
        0x200041204200e000ull,
        0x000200132121042eull,
        0x000100010005002eull,
        0x223e014100200000ull,
        0x131e025200110000ull,
        0xe431120001000000ull,
        0x124e246212000100ull,
        0x000000010232112eull,
        0x000001200230231eull,
        0x200030004210e000ull,
        0x000000001020e431ull,
        0x000001003300e131ull,
        0xe100421030001000ull,
        0x122e003200410000ull,
        0xe300242021001000ull,
        0x012e027200020012ull,
        0x100041201420e410ull,
        0x000100120143012eull,
        0x224e102200100000ull,
        0x031e043301110000ull,
        0x000000000130124eull,
        0xe000142012002000ull,
        0x010002001110e332ull,
        0x000003001120e240ull,
        0x001200330041002eull,
        0x000000120034001eull,
        0x010003000220114eull,
        0x000020003200e421ull,
        0xe100230043002000ull,
        0x000100030234001eull,
        0x100021004212e100ull,
        0xe332042102000100ull,
        0xe431020001000000ull,
        0x103e004200000000ull,
        0x025e013200100000ull,
        0x244e102200100000ull,
        0x000000100032124eull,
        0x000e212300120021ull,
        0xe321341011000000ull,
        0x000002101320e431ull,
        0x002100130134000eull,
        0xe431010200000000ull,
        0x134e003200000000ull,
        0x000201320004000eull,
        0x000e002400220002ull,
        0x000030003400e210ull,
        0xe422101000000000ull,
        0xe200241010000000ull,
        0x000000000101234eull,
        0x000100120243001eull,
        0xe000421021001000ull,
        0x000e002400210121ull,
        0xe200423021000000ull,
        0x02120044002e0002ull,
        0x34104000e1002000ull,
        0x124e012100000000ull,
        0x000100020143001eull,
        0x002e013400120001ull,
        0x233e012100100000ull,
        0x113e032000100000ull,
        0xe130331010000000ull,
        0x000020101030232eull,
        0xe421111000000000ull,
        0x00001200240023e2ull,
        0xe000312032001000ull,
        0x000001002210e421ull,
        0x200022003300e200ull,
        0x001e034300020002ull,
        0x012e002402120001ull,
        0x000100130124001eull,
        0x034e003200110000ull,
        0x002e132100140002ull,
        0x23110022004e0000ull,
        0x001e042300020001ull,
        0xe321240001000000ull,
        0x34e1230200000000ull,
        0x000013102420e300ull,
        0x000200120024001eull,
        0x000000100122124eull,
        0x124e011400000000ull,
        0x000101210034000eull,
        0x100013003400e100ull,
        0x000200230132002eull,
        0x000011003400e321ull,
        0x000002003300e241ull,
        0x243e222000000000ull,
        0x000200120034000eull,
        0x0221004e01230000ull,
        0xe520110000000000ull,
        0x000e012400030002ull,
        0x000000100130034eull,
        0x000010002300e420ull,
        0xe222041002000100ull,
        0xe200430031001200ull,
        0x000e012400120001ull,
        0x22104320e1002000ull,
        0xe142230101000000ull,
        0x124e012100000000ull,
        0x000010002120143eull,
        0x100031004200e000ull,
        0x000000003310e421ull,
        0x001e002400020001ull,
        0x001e001401320021ull,
        0x000000010014133eull,
        0xe212130004000200ull,
        0x133e204000200000ull,
        0xe000430021001000ull,
        0x121031004000e000ull,
        0x002e014100130001ull,
        0xe421121001000000ull,
        0x000000400022023eull,
        0x000100210042000eull,
        0x0031003e00140002ull,
        0x000101230033000eull,
        0xe400232121000000ull,
        0x000002002300e233ull,
        0x143e202200000000ull,
        0x000001000210e432ull,
        0xe430020001000000ull,
        0x200031004100e000ull,
        0x002e000300240002ull,
        0xe300232020001000ull,
        0xe000421031001000ull,
        0x0000200042001e21ull,
        0x002100310032000eull,
        0x144e016200100000ull,
        0x00002000131025e2ull,
        0x000100120323001eull,
        0x002e013400120001ull,
        0x000100220024001eull,
        0x000000110243113eull,
        0x021e002400120002ull,
        0x031e004300110000ull,
        0xe430121000000000ull,
        0xe000234012301000ull,
        0x000200230134000eull,
        0x100012102400e000ull,
        0x24e2131100020001ull,
        0x200021003210e100ull,
        0x000102120124000eull,
        0x000100020124012eull,
        0xe332120001000000ull,
        0x100020004310e100ull,
        0x000002003400e221ull,
        0x124e012200100000ull,
        0x134e003000200000ull,
        0x100020003420e100ull,
        0x002e015300130000ull,
        0x014e023000100000ull,
        0x001003200240113eull,
        0x000004200240211eull,
        0x000e014100040021ull,
        0xe211032200000000ull,
        0x002e001400130001ull,
        0x13104200e2001000ull,
        0x210032004200e000ull,
        0x000011002400e321ull,
        0x000e021400120001ull,
        0x000000200111242eull,
        0xe100420031002000ull,
        0xe211130034000000ull,
        0x210041202120e000ull,
        0x133e004100000000ull,
        0x223e002200300000ull,
        0x000000200011233eull,
        0x112e003300400000ull,
        0x000031004320e100ull,
        0x010002004500e111ull,
        0x000000100132141eull,
        0xe332310020000000ull,
        0x000000002400e321ull,
        0x000020003200e401ull,
        0x00000012003204e1ull,
        0x000200210421000eull,
        0x000000220124000eull,
        0xe430130001000000ull,
        0x132e003200010000ull,
        0xe000421021001000ull,
        0x2412002e00240000ull,
        0x0001002e00640312ull,
        0xe300342030001000ull,
        0x000000110043113eull,
        0x100031004100e000ull,
        0x000000000310e422ull,
        0xe421130000000000ull,
        0x000000300021223eull,
        0x100021004210e120ull,
        0x000000120034002eull,
        0x000000100042033eull,
        0x000021004320e200ull,
        0xe000422020001000ull,
        0xe221341203000000ull,
        0xe331021001000000ull,
        0x100020004220e000ull,
        0x000100220034012eull,
        0x002000300223111eull,
        0x134e012100000000ull,
        0xe300242020000000ull,
        0x100021004200e120ull,
        0x2431001e00030001ull,
        0x012e001300340000ull,
        0x200021004210e120ull,
        0x000100130213001eull,
        0xe430030001000000ull,
        0x002e023100140002ull,
        0x000001000200e432ull,
        0xe200421022000000ull,
        0x000000120005001eull,
        0x000001001200e421ull,
        0x000100143412001eull,
        0x2220e20040001000ull,
        0xe120432020001000ull,
        0x000000110123123eull,
        0x2000e30043103120ull,
        0x000000000102025eull,
        0x000000001120224eull,
        0xe421121023000000ull,
        0x100020004310e000ull,
        0xe100340013201000ull,
        0x001e002400120021ull,
        0x000000400231112eull,
        0x000000400030122eull,
        0x000000000101125eull,
        0x121023104100e000ull,
        0x100022004210e100ull,
        0xe420121020000000ull,
        0x000013003230e000ull,
        0x000100120143002eull,
        0x000200220243013eull,
        0x000200210421000eull,
        0x002000100123231eull,
        0xe000430021001000ull,
        0x133e014100300000ull,
        0xe210420020002000ull,
        0xe000410031001000ull,
        0x000e012300220002ull,
        0x1e54013100020000ull,
        0xe331240001000000ull,
        0x210042003000e000ull,
        0x000002001410e320ull,
        0x000001000230114eull,
        0x222e003100400000ull,
        0x124e012100100000ull,
        0x112e003100400010ull,
        0x000100030043001eull,
        0x000032004310e210ull,
        0x000011002300e431ull,
        0x000100220123001eull,
        0xe100421030000000ull,
        0x000000200111142eull,
        0xe321122001000000ull,
        0xe321241001000000ull,
        0x100021004200e100ull,
        0x000002002402e321ull,
        0x000200130231002eull,
        0x000010002400e301ull,
        0x200013002142e300ull,
        0x000100420142002eull,
        0x100041203300e200ull,
        0xe241233000100000ull,
        0xe000124013002100ull,
        0x00002100132012e0ull,
        0x322e124000100000ull,
        0x02330005002e0002ull,
        0x000000002220e431ull,
        0x000020004310e200ull,
        0x012e214200230001ull,
        0x210032001510e200ull,
        0x100042004200e100ull,
        0xe000234020001000ull,
        0x002e002301320001ull,
        0x000100220124000eull,
        0x123e012300120000ull,
        0x2350e00032002000ull,
        0x000001000200e431ull,
        0x000001230134002eull,
        0x000000002210e421ull,
        0xe422220000000000ull,
        0x000002003300e231ull,
        0x224e111000000000ull,
        0xe321324001000000ull,
        0x1220e20042001000ull,
        0x100020004100e200ull,
        0x000e012400230000ull,
        0x132e014200010000ull,
        0x000100020432001eull,
        0x001e002400120001ull,
        0x000000410132132eull,
        0x124e012300000000ull,
        0x122e243103200000ull,
        0xe112022004000000ull,
        0x000200230241023eull,
        0x134e222101000000ull,
        0x000000100322224eull,
        0x001000200052123eull,
        0x000000011312e431ull,
        0x000000100023134eull,
        0x000e012400120021ull,
        0x003e002400220000ull,
        0x000000100220024eull,
        0x2000e00051003320ull,
        0xe142042000100000ull,
        0x000000300140121eull,
        0x000200030123003eull,
        0xe000234012001000ull,
        0xe000422020001000ull,
        0xe441221010000000ull,
        0x3100e00053003132ull,
        0x234e020100000000ull,
        0x000200030124000eull,
        0xe331240011000000ull,
        0x034e012000100000ull,
        0x000002001300e422ull,
        0x001200320124023eull,
        0x003e024301320000ull,
        0x000000100342222eull,
        0x000e013400230002ull,
        0xe100342022000000ull,
        0xe420122001000000ull,
        0xe310400020001000ull,
        0xe000310041002000ull,
        0x234e113200010000ull,
        0x001e021300130001ull,
        0x000100030143001eull,
        0x122024e020001000ull,
        0x000000100042033eull,
        0x100020004300e200ull,
        0x000000000121025eull,
        0xe321233010000000ull,
        0x000000220254002eull,
        0x100031004100e100ull,
        0x012100010034000eull,
        0x023e004200110000ull,
        0xe432110002000000ull,
        0x012e004200120001ull,
        0x012e004100130001ull,
        0x000e021300130012ull,
        0x034e006300120000ull,
        0x002e004300020001ull,
        0x000001000211e432ull,
        0x000e021300030012ull,
        0x000200320034000eull,
        0x000e042100210002ull,
        0xe420242000000000ull,
        0x131e023300010000ull,
        0x200041003200e100ull,
        0x000020003410e200ull,
        0x001e014300020001ull,
        0x100032004321e200ull,
        0x200031002300e100ull,
        0x200031003120e000ull,
        0x100032005200e000ull,
        0xe421251001000000ull,
        0x123e004300100000ull,
        0xe200132031002000ull,
        0x024e013100100000ull,
        0x000100310035000eull,
        0x000000000101125eull,
        0xe100142013201000ull,
        0x000200032124001eull,
        0x002e002400120001ull,
        0x2e20120041001000ull,
        0x020001003211e132ull,
        0x100031004210e000ull,
        0x100031002430e200ull,
        0x100032102400e210ull,
        0x000012102410e310ull,
        0x002e004300120000ull,
        0x000000300310222eull,
        0x2e31004300310000ull,
        0x000003002220e422ull,
        0x000000100131134eull,
        0x000001003310e431ull,
        0x200021004300e000ull,
        0x000000140031013eull,
        0x012e004200020000ull,
        0xe141133200000000ull,
        0xe422011002000000ull,
        0x000000000111234eull,
        0x000e012400220001ull,
        0x002000100123231eull,
        0x000001002200e420ull,
        0x000000131342003eull,
        0xe321240000000000ull,
        0xe000421021001000ull,
        0x000000010005002eull,
        0xe122343001000000ull,
        0x002100210004000eull,
        0xe200131032001000ull,
        0x000000001220e421ull,
        0x000012003400e310ull,
        0x000000220353113eull,
        0x000100120432002eull,
        0xe121423011100000ull,
        0x000200120024001eull,
        0xe200431030000000ull,
        0x134e003000100000ull,
        0x000000120133131eull,
        0x000100131241024eull,
        0x002e002400220021ull,
        0xe432020001000000ull,
        0x000001002310e430ull,
        0xe342102000000000ull,
        0xe200341020000000ull,
        0x010003001210e321ull,
        0x00000000023214e2ull,
        0x000000000022234eull,
        0x124e022100100000ull,
        0x014e013100020001ull,
        0xe210510032001000ull,
        0x134e012100100000ull,
        0x003e123100030001ull,
        0xe331140000000000ull,
        0x000021004210e100ull,
        0xe100410020001000ull,
        0x000000400330e112ull,
        0x000e043201210001ull,
        0x000002001110e241ull,
        0x000e002400230001ull,
        0x000000400031112eull,
        0x131e003200110000ull,
        0x000100020142002eull,
        0xe142022001000200ull,
        0x030002000310e132ull,
        0x000001000200e431ull,
        0x001e002400120002ull,
        0x000300e100430312ull,
        0xe321420011000000ull,
        0x232e023200100000ull,
        0x000100020034001eull,
        0x002100140142002eull,
        0x241e223100100000ull,
        0x000100020043002eull,
        0x124e202200100000ull,
        0x000011003401e242ull,
        0xe200321031001000ull,
        0x000100030024001eull,
        0x13e1034400030002ull,
        0x241e113100100000ull,
        0x000100320034000eull,
        0x200031004000e100ull,
        0x224e012100000000ull,
        0x000002001341e112ull,
        0xe200420021001000ull,
        0x000021003400e321ull,
        0x124e022100100000ull,
        0x000001000400114eull,
        0x2000e40054233142ull,
        0xe000341020002000ull,
        0x000000110042113eull,
        0x004e002100120001ull,
        0x2e52104200120000ull,
        0xe000421030001000ull,
        0x15e2220011000000ull,
        0x002e014200020002ull,
        0x000010002000134eull,
        0x1432232e00010000ull,
        0x000004001310e220ull,
        0x120040003100e200ull,
        0xe421011000000000ull,
        0x000000101130e422ull,
        0x000000102420e211ull,
        0xe200342023000000ull,
        0x214e103200200000ull,
        0x000e003400030001ull,
        0x002e002400240001ull,
        0x100021004210e100ull,
        0x001e012400120000ull,
        0x000004100220e112ull,
        0xe200132040000000ull,
        0xe322123000100000ull,
        0x002e024200120000ull,
        0xe310131030001000ull,
        0xe410342020001000ull,
        0x200030004210e000ull,
        0x012e013200230012ull,
        0x001e003400030001ull,
        0x000e003400120002ull,
        0x000000121323002eull,
        0xe200400021001000ull,
        0x000000001300e421ull,
        0xe431020002000000ull,
        0x000000002000e430ull,
        0x232e013200100000ull,
        0x00030045000e0002ull,
        0xe141032200010000ull,
        0x023e003302320002ull,
        0xe520110000000000ull,
        0x000020004300e100ull,
        0xe332240010000000ull,
        0xe100470042102000ull,
        0x022e013100400000ull,
        0xe322321020000000ull,
        0x00003300340011e1ull,
        0x120020002340e000ull,
        0x100023202321e000ull,
        0xe141340011000000ull,
        0x000000110043131eull,
        0xe100420031000000ull,
        0xe130431020001000ull,
        0x134e012100100000ull,
        0x134e022200000000ull,
        0x000001000430e322ull,
        0x000001001300e431ull,
        0x000000200241031eull,
        0x00004000124012e0ull,
        0x003e013100130003ull,
        0x001e003400230001ull,
        0x125e010200000000ull,
        0x200023004100e000ull,
        0x002e013200130001ull,
        0xe223030004000000ull,
        0x000002200133232eull,
        0x200021003410e210ull,
        0xe100430030001000ull,
        0x000000002100144eull,
        0x000000000120134eull,
        0xe420122001000000ull,
        0xe100432030002000ull,
        0x123e022300100000ull,
        0xe200431012101000ull,
        0x000000120023024eull,
        0xe310240001000000ull,
        0x001e002400130001ull,
        0x00001200234012e0ull,
        0xe000420030001000ull,
        0x000010004300e122ull,
        0x1412e31020000000ull,
        0x200030004210e000ull,
        0xe210341010002000ull,
        0x000e014300210002ull,
        0x012e014300030000ull,
        0xe321340001000000ull,
        0xe441320001000000ull,
        0xe300240001000000ull,
        0x0312003e00230002ull,
        0xe200500021002000ull,
        0xe431230011000000ull,
        0x234e002100100000ull,
        0x214e040100020000ull,
        0x012e212400420002ull,
        0x000101322413e221ull,
        0x114e023001100000ull,
        0x100031002410e310ull,
        0x000100120024001eull,
        0xe200420021001000ull,
        0x132e263012200010ull,
        0x00000011003314e1ull,
        0xe332011103000000ull,
        0x200021003400e100ull,
        0xe420230000000000ull,
        0x000001310242232eull,
        0x00002100240024e2ull,
        0x000102311342001eull,
        0xe100500020002000ull,
        0x000001000202e431ull,
        0xe221030004000000ull,
        0xe120420040000000ull,
        0x241e133200110000ull,
        0x000000010143003eull,
        0x132e014302010000ull,
        0x000100330131003eull,
        0x000002102320e142ull,
        0xe000234022001000ull,
        0x121e033000400000ull,
        0x000000000141223eull,
        0x000000101030224eull,
        0x000000013121014eull,
        0x000000000220e421ull,
        0x000000002310e241ull,
        0x000001000200e431ull,
        0x124e003100000000ull,
        0x012e003101240001ull,
        0x100020003400e320ull,
        0x21214400e0003000ull,
        0xe000300024102100ull,
        0x000001001310e431ull,
        0x000000200222141eull,
        0xe000121213102100ull,
        0x100021003410e000ull,
        0x100021003410e210ull,
        0x200020004100e100ull,
        0x001e012400020001ull,
        0x000100020134001eull,
        0x000000000312224eull,
        0xe321041001000000ull,
        0x000000030142000eull,
        0xe200420032000000ull,
        0x100021003421e000ull,
        0x001e002400230012ull,
        0x00003000e2101430ull,
        0x000000210025013eull,
        0x000000000121134eull,
        0x000000200020e410ull,
        0x010012002140123eull,
        0x010012003300e220ull,
        0xe100421020001000ull,
        0x25e2230120000000ull,
        0xe431120001000000ull,
        0x100021005200e100ull,
        0x023e042201110000ull,
        0x200021003420e100ull,
        0x000300320032001eull,
        0xe200562032001000ull,
        0x000000000021134eull,
        0x112e032000400000ull,
        0x000100242321002eull,
        0x002100120024002eull,
        0xe100312130001000ull,
        0x002100020134000eull,
        0x023e014200110000ull,
        0xe532131100000000ull,
        0x000100210132002eull,
        0x233e140001000000ull,
        0xe000420031002000ull,
        0x000000200211024eull,
        0x210041002200e000ull,
        0x141e004200100000ull,
        0x000002000110e422ull,
        0x100021004100e000ull,
        0x000000000311e232ull,
        0xe200341012001000ull,
        0x010002003430e141ull,
        0x000000001020e431ull,
        0x000000130014012eull,
        0x134e013200000000ull,
        0x002e023400030001ull,
        0x000000000041123eull,
        0x024e012000100000ull,
        0x000e012400120001ull,
        0x000000110144032eull,
        0x0000200013302e41ull,
        0xe231231001000000ull,
        0x000000020121124eull,
        0x330021003410e000ull,
        0x000100140132001eull,
        0x001e012400120001ull,
        0xe000430031001000ull,
        0xe421030001000000ull,
        0x024e002200300000ull,
        0xe331350202000000ull,
        0x010003003400e142ull,
        0x03231234001e0003ull,
        0x003e034200210001ull,
        0x001e034300120001ull,
        0x013e004301310001ull,
        0x211e024000200000ull,
        0x000000200232131eull,
        0x001200210041000eull,
        0x000000000120134eull,
        0x000200210421000eull,
        0x000001001200e421ull,
        0x100020004400e100ull,
        0x000100130134012eull,
        0x000011002420e141ull,
        0x000001000200e431ull,
        0x000000120123123eull,
        0x030001001210e122ull,
        0xe000421021001000ull,
        0x000002001300e432ull,
        0x124e020000000000ull,
        0x000000200111034eull,
        0x002100030014000eull,
        0x000000000122133eull,
        0x120020002340e000ull,
        0xe310141042101000ull,
        0xe321340001000000ull,
        0x000011002310e420ull,
        0xe210341020002000ull,
        0x000011002400e330ull,
        0x000200030024002eull,
        0x002e012300040000ull,
        0x000001000410e321ull,
        0x001000200141021eull,
        0x123e013100000000ull,
        0x000000010113033eull,
        0x015e001100020000ull,
        0x000100210134012eull,
        0x000e043100010001ull,
        0xe200432031000000ull,
        0xe232331010000000ull,
        0x000200020024002eull,
        0x000100040023002eull,
        0x000200130234000eull,
        0x200012101340e000ull,
        0xe341222000100000ull,
        0x000000120124001eull,
        0x000002002520e142ull,
        0xe421131002000000ull,
        0xe320224010200000ull,
        0x100012001230e400ull,
        0x000000201033222eull,
        0x001e012400120001ull,
        0x222e003400010000ull,
        0xe311340011000000ull,
        0x000002102320e232ull,
        0x000000010002134eull,
        0x224e102100000000ull,
        0x001e121300130002ull,
        0x012e015301210000ull,
        0x000e012400030001ull,
        0x241e014200200000ull,
        0x033e004200000000ull,
        0x000031003400e200ull,
        0x001200030131013eull,
        0x000000102020134eull,
        0x000000201111242eull,
        0x000000001400e322ull,
        0x014e004200120000ull,
        0xe120033004000000ull,
        0x013e004300210010ull,
        0x000100031241002eull,
        0x000022003400e310ull,
        0x000100130023012eull,
        0x000100030004013eull,
        0x2e42001100030001ull,
        0x243e111002000100ull,
        0x123e004100000000ull,
        0x012e013400030000ull,
        0x000000100231134eull,
        0x000100120024001eull,
        0x010002001310e430ull,
        0x000001210034001eull,
        0x000e023400030002ull,
        0x000020003410e210ull,
        0x200031001241e200ull,
        0x141e013300100000ull,
        0x000021001243e100ull,
        0x000100022124001eull,
        0x002e014300310000ull,
        0x121e013400110000ull,
        0x112e023201000000ull,
        0x100012101240e000ull,
        0x00210042003100e1ull,
        0x000010002410122eull,
        0xe000420021002100ull,
        0x100031004200e000ull,
        0x000000000320e431ull,
        0x000100220124000eull,
        0x234e002000100000ull,
        0x021e000500130000ull,
        0x000100320024012eull,
        0x000100020025001eull,
        0xe331240012000000ull,
        0xe000431030002000ull,
        0x000001000301124eull,
        0xe521021002000000ull,
        0x000000200142233eull,
        0x10004100e2002210ull,
        0x000200030034012eull,
        0x000e043200020022ull,
        0x000020002341e210ull,
        0xe400420022000000ull,
        0x200042001530e200ull,
        0xe430143020000000ull,
        0x001200320213000eull,
        0x000100010142012eull,
        0xe000124012301000ull,
        0x200030004210e100ull,
        0x123e004200000000ull,
        0x000e012400020001ull,
        0x000000020143012eull,
        0xe000124012002000ull,
        0x000001001220e421ull,
        0xe100421021001000ull,
        0xe431020000000000ull,
        0x000000000221e342ull,
        0xe421330010000000ull,
        0xe000431020002000ull,
        0x000000120121024eull,
        0xe210151030001000ull,
        0xe300320030001210ull,
        0x000000000220124eull,
        0xe432112000000000ull,
        0xe221240010000000ull,
        0x000000120041024eull,
        0xe412220110000000ull,
        0x000100310242002eull,
        0x000000200142131eull,
        0x000032001510e200ull,
        0xe132332002000000ull,
        0x200031004200e000ull,
        0xe000241030001000ull,
        0x000000101030e421ull,
        0x200020003410e000ull,
        0xe000420020001200ull,
        0xe421120000000000ull,
        0xe300420012101000ull,
        0x121e014000300000ull,
        0x012100030004000eull,
        0xe421120001000000ull,
        0x000000200121024eull,
        0xe141032000200000ull,
        0x134e015200100000ull,
        0xe421233000000000ull,
        0xe421021001000000ull,
        0x1212e40032002000ull,
        0xe120130042002000ull,
        0x000100020032014eull,
        0x000e014300020001ull,
        0xe100421021001000ull,
        0xe300342010001200ull,
        0x034e023201010000ull,
        0x02130124004e0010ull,
        0x100021004210e100ull,
        0x000100130014001eull,
        0x000100130053013eull,
        0xe000141012302000ull,
        0x000000300121124eull,
        0x012e013400030002ull,
        0x00003e0012303120ull,
        0x100021003210e310ull,
        0x000100020124002eull,
        0x000000121242002eull,
        0x0000000304220e21ull,
        0x124e003100000000ull,
        0x001e004300020001ull,
        0xe312042001000000ull,
        0x141e002300110000ull,
        0x000200220004000eull,
        0xe100430031000000ull,
        0x000000100021024eull,
        0x040003202532e143ull,
        0x23e1351012000000ull,
        0x000000300112034eull,
        0x000000200110233eull,
        0x012e212400130001ull,
        0x231e023300110000ull,
        0xe420120001000000ull,
        0x002e014101240002ull,
        0x000012002340e000ull,
        0x000000030134002eull,
        0xe100421030002000ull,
        0xe300124310002000ull,
        0x000000200120024eull,
        0x000000100030134eull,
        0x20003100e3003121ull,
        0xe100320042002000ull,
        0x014e002300240000ull,
        0x01230124000e0002ull,
        0x012e002400030002ull,
        0xe432020001000000ull,
        0x200031002310e310ull,
        0xe100421021001200ull,
        0x100021004210e200ull,
        0x000e034300030002ull,
        0x24e2131031002000ull,
        0x124e011100200000ull,
        0x100012102430e210ull,
        0x000100120124023eull,
        0x000100020124001eull,
        0x025e012200000000ull,
        0x231e024200310000ull,
        0x224e002000100000ull,
        0x125e002100000000ull,
        0x134e012200000000ull,
        0xe400131021001000ull,
        0x000001000210e432ull,
        0x35e1130031000000ull,
        0x003e034200010001ull,
        0x232e104100000000ull,
        0x000e002400030002ull,
        0xe100312030003000ull,
        0x133e015200200000ull,
        0x114e001400000000ull,
        0x00000014002e0221ull,
        0x23e1003300020001ull,
        0x200030001310e300ull,
        0x021e024300010021ull,
        0x000100032124000eull,
        0xe300131031002100ull,
        0x000100030024002eull,
        0xe422031000000000ull,
        0x000000100122124eull,
        0x000000200152033eull,
        0xe200341020000000ull,
        0x243e022100000000ull,
        0x100020004210e100ull,
        0x100021204220e000ull,
        0x200040003212e000ull,
        0x000100130024001eull,
        0x000002003410e220ull,
        0x000200020024002eull,
        0x010003300410e122ull,
        0x000000002301e432ull,
        0x002e124300220002ull,
        0xe322023000100000ull,
        0x134e011200000000ull,
        0xe300321031001000ull,
        0x000e013400210002ull,
        0x120012004300e000ull,
        0x13420001000e0000ull,
        0xe122161004300000ull,
        0xe321240000000000ull,
        0x123e004100000000ull,
        0x000001000332241eull,
        0xe000421030002000ull,
        0x100030004200e000ull,
        0x000000010133124eull,
        0xe100422031000000ull,
        0x003e212100030003ull,
        0x100031004310e100ull,
};

template<u8 N>
Game_stat training_episode() {
    vector<r_t> evals;
    vector<s_t> rewards;
    vector<u64> afterstates;
    s_t score = 0;

    u64 board = 0;
    //fill_board<N>(board);
    //fill_board<N>(board);
    uniform_int_distribution<u32> dist(0, stage_starts.size() - 1);
    board = stage_starts[dist(rng)];
    while (true) {
        const auto [dir, eval, reward, afterstate]
                = eval_state<N>(board);
        if (dir == None) { break; }
        evals.push_back(eval);
        rewards.push_back(reward);
        afterstates.push_back(afterstate);
        score += reward;

        board = afterstate;
        fill_board<N>(board);
    }
    const u32 moves = evals.size();

    r_t scaled_learning_rate = learning_rate / 8;
    if constexpr (N == 4) {
        scaled_learning_rate /= r_t(tuples_size_4);
    } else {
        scaled_learning_rate /= r_t(tuples_size_3);
    }

    r_t target = 0;
    for (u32 t = moves - 1; t < moves; --t) {
        const r_t error = target - add_weights<N>(afterstates[t]);
        target = r_t(rewards[t]) + update_weights<N>(afterstates[t], error * scaled_learning_rate);

        //const r_t error = target - evals[t];
        //update_weights<N>(afterstates[t], error * scaled_learning_rate);
        //target = r_t(rewards[t]) + evals[t];
    }

    return {board, score, moves};
}

template<u8 N>
Game_stat testing_episode() {
    u64 board = 0;
    s_t score = 0;
    u32 moves = 0;

    //fill_board<N>(board);
    //fill_board<N>(board);
    uniform_int_distribution<u32> dist(0, stage_starts.size() - 1);
    board = stage_starts[dist(rng)];
    while (true) {
        const auto [dir, eval, reward, afterstate]
                = eval_state<N>(board);
        if (dir == None) { break; }
        score += reward;
        ++moves;

        board = afterstate;
        fill_board<N>(board);
    }

    return {board, score, moves};
}

template<u8 N>
vector<Game_stat> run_training_episodes(u32 games, u8 threads) {
    srand(42);
    cout << "Training started (" << games << " games)" << endl;

    run_stats = {};
    vector<Game_stat> games_stats;
    auto start = time_now();
    if (threads == 0) {
        for (u32 i = 0; i < games; i++) {
            games_stats.push_back(training_episode<N>());
        }
    } else {
        vector<thread> all_threads;
        const u32 threads_games = games / threads;
        vector<vector<Game_stat>> threads_stats(threads);
        for (u8 t = 0; t < threads; ++t) {
            all_threads.emplace_back([t, threads_games, &threads_stats]() {
                for (u32 i = 0; i < threads_games; ++i) {
                    threads_stats[t].push_back(training_episode<N>());
                }
            });
        }
        for (auto &thread: all_threads) {
            thread.join();
        }
        for (const auto &thread_stats: threads_stats) {
            games_stats.insert(games_stats.end(), thread_stats.begin(), thread_stats.end());
        }
    }
    training_stats = {};
    for (const auto &game_stats: games_stats) {
        training_stats.update_board_stats(game_stats.board, game_stats.score, game_stats.moves);
    }
    r_t elapsed = time_since(start);

    if (DEBUG) {
        run_stats.print_board_operation_stats(elapsed);
        run_stats.print_eval_operation_stats(elapsed);

        training_stats.print_average_game_stats();
        training_stats.print_max_game_stats();
        training_stats.print_score_cell_stats();
    } else {
        //cout << indent << "Move per us:    \t" << r_t(run_stats.move_board_counter) / elapsed << endl;
        //cout << indent << "Eval per us:    \t" << r_t(run_stats.eval_board_counter) / elapsed << endl;
        //cout << indent << "Update per us:  \t" << r_t(run_stats.update_weights_counter) / elapsed << endl;
        cout << indent << "Ms per game:    \t" << (elapsed / 1e3) / r_t(training_stats.game_counter) << endl;
        cout << indent << "Move per game:  \t" << r_t(training_stats.moves_counter) / r_t(training_stats.game_counter) << endl;
    }

    cout << "Training finished (" << time_since(start) / 1e6 << " s)" << endl;

    return games_stats;
}

template<u8 N>
vector<Game_stat> run_testing_episodes(u32 games, u8 threads) {
    srand(42);
    cout << "Testing started (" << games << " games)" << endl;

    run_stats = {};
    vector<Game_stat> games_stats;
    auto start = time_now();
    if (threads == 0) {
        for (u32 i = 0; i < games; i++) {
            games_stats.push_back(testing_episode<N>());
        }
    } else {
        vector<thread> all_threads;
        const u32 threads_games = games / threads;
        vector<vector<Game_stat>> threads_stats(threads);
        for (u8 t = 0; t < threads; ++t) {
            all_threads.emplace_back([t, threads_games, &threads_stats]() {
                for (u32 i = 0; i < threads_games; ++i) {
                    threads_stats[t].push_back(testing_episode<N>());
                }
            });
        }
        for (auto &thread: all_threads) {
            thread.join();
        }
        for (const auto &thread_stats: threads_stats) {
            games_stats.insert(games_stats.end(), thread_stats.begin(), thread_stats.end());
        }
    }
    testing_stats = {};
    for (const auto &game_stats: games_stats) {
        testing_stats.update_board_stats(game_stats.board, game_stats.score, game_stats.moves);
    }
    r_t elapsed = time_since(start);

    if (DEBUG) {
        run_stats.print_board_operation_stats(elapsed);
        run_stats.print_eval_operation_stats(elapsed);

        testing_stats.print_average_game_stats();
        testing_stats.print_max_game_stats();
        testing_stats.print_score_cell_stats();
    } else {
        //testing_stats.print_score_cell_stats();
        //cout << indent << "Move per us:    \t" << r_t(run_stats.move_board_counter) / elapsed << endl;
        //cout << indent << "Eval per us:    \t" << r_t(run_stats.eval_board_counter) / elapsed << endl;
        cout << indent << "Ms per game:    \t" << (elapsed / 1e3) / r_t(testing_stats.game_counter) << endl;
        cout << indent << "Move per game:  \t" << r_t(testing_stats.moves_counter) / r_t(testing_stats.game_counter) << endl;
        cout << indent << "Score per game: \t" << r_t(testing_stats.score_counter) / r_t(testing_stats.game_counter) << endl;
    }

    cout << "Testing finished (" << time_since(start) / 1e6 << " s)" << endl;

    return games_stats;
}

template<u8 N>
void run_learning(u32 episodes, u32 training_games, u32 testing_games, u8 threads) {
    cout << "Learning started (" << episodes << " episodes, LR=" << learning_rate << ")" << endl;

    run_stats_t all_run_stats = {};
    game_stats_t all_training_stats = {};
    game_stats_t all_testing_stats = {};

    run_testing_episodes<N>(testing_games, threads);
    cout << endl;

    auto start = time_now();
    for (u32 i = 0; i < episodes; i++) {
        cout << "Episode " << i + 1 << endl;

        run_training_episodes<N>(training_games, threads);
        all_training_stats.append_stats(training_stats);
        all_run_stats.append_stats(run_stats);

        run_testing_episodes<N>(testing_games, threads);
        all_run_stats.append_stats(run_stats);
        all_testing_stats.append_stats(testing_stats);

        cout << endl;
    }
    r_t elapsed = time_since(start);

    if (DEBUG) {
        /*cout << "Overall run stats per s" << endl;
        cout << indent << "Fill board:     \t" << r_t(all_run_stats.fill_board_counter) / elapsed << endl;
        cout << indent << "Reward board:   \t" << r_t(all_run_stats.reward_board_counter) / elapsed << endl;
        cout << indent << "Move board:     \t" << r_t(all_run_stats.move_board_counter) / elapsed << endl;
        cout << indent << "Eval board:     \t" << r_t(all_run_stats.eval_board_counter) / elapsed << endl;
        cout << indent << "Update weights: \t" << r_t(all_run_stats.update_weights_counter) / elapsed << endl;
        cout << indent << "Eval moves:     \t" << r_t(all_run_stats.eval_moves_counter) / elapsed << endl;

        cout << "Overall training stats per game (" << all_training_stats.game_counter << " games)" << endl;
        cout << indent << "Score per game: \t" << r_t(all_training_stats.score_counter) / r_t(all_training_stats.game_counter) << endl;
        cout << indent << "Moves per game: \t" << r_t(all_training_stats.moves_counter) / r_t(all_training_stats.game_counter) << endl;

        cout << "Overall testing stats per game (" << all_testing_stats.game_counter << " games)" << endl;
        cout << indent << "Score per game: \t" << r_t(all_testing_stats.score_counter) / r_t(all_testing_stats.game_counter) << endl;
        cout << indent << "Moves per game: \t" << r_t(all_testing_stats.moves_counter) / r_t(all_testing_stats.game_counter) << endl;

        cout << indent << "Max moves:      \t" << all_testing_stats.max_moves << endl;
        cout << indent << "Max score:      \t" << all_testing_stats.max_score << endl;
        cout << indent << "Max cell:       \t" << all_testing_stats.max_cell << endl;
        cout << indent << "Score A:        \t" << r_t(all_testing_stats.score_A_counter) / r_t(all_testing_stats.game_counter) << endl;
        cout << indent << "Score B:        \t" << r_t(all_testing_stats.score_B_counter) / r_t(all_testing_stats.game_counter) << endl;
        cout << indent << "Score C:        \t" << r_t(all_testing_stats.score_C_counter) / r_t(all_testing_stats.game_counter) << endl;
        cout << indent << "Score D:        \t" << r_t(all_testing_stats.score_D_counter) / r_t(all_testing_stats.game_counter) << endl;
        cout << indent << "Score E:        \t" << r_t(all_testing_stats.score_E_counter) / r_t(all_testing_stats.game_counter) << endl;
        cout << indent << "Score F:        \t" << r_t(all_testing_stats.score_F_counter) / r_t(all_testing_stats.game_counter) << endl;
        cout << indent << "Score G:        \t" << r_t(all_testing_stats.score_G_counter) / r_t(all_testing_stats.game_counter) << endl;
        cout << indent << "Score H:        \t" << r_t(all_testing_stats.score_H_counter) / r_t(all_testing_stats.game_counter) << endl;*/

        cout << "Overall run stats (" << elapsed << " us)" << endl;
        all_run_stats.print_board_operation_stats(elapsed);
        all_run_stats.print_eval_operation_stats(elapsed);

        cout << "Overall training stats (" << all_training_stats.game_counter << " games)" << endl;
        all_training_stats.print_average_game_stats();
        all_training_stats.print_max_game_stats();
        all_training_stats.print_score_cell_stats();

        cout << "Overall testing stats (" << all_testing_stats.game_counter << " games)" << endl;
        all_testing_stats.print_average_game_stats();
        all_testing_stats.print_max_game_stats();
        all_testing_stats.print_score_cell_stats();
    }

    cout << "Learning finished (" << elapsed / 1e6 << " s)" << endl;
    cout << endl;
}

template<u8 N>
void fixed_learn(r_t LR, u32 episodes, u32 training_games, u32 testing_games, u8 threads) {
    learning_rate = LR;
    run_learning<N>(episodes, training_games, testing_games, threads);
    string ts_str = get_time_str();
    cout << "Timestamp: " << ts_str << endl;
    save_packed_weights<N>(ts_str);
    cout << endl;
}
