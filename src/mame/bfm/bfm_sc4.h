// license:BSD-3-Clause
// copyright-holders:David Haywood
/* Scorpion 4 + 5 driver related includes */
/* mainly used for stuff which is currently shared between sc4 / 5 sets to avoid duplication */
#ifndef MAME_INCLUDES_BFP_SC4_H
#define MAME_INCLUDES_BFP_SC4_H

#pragma once

#include "sec.h"
#include "machine/steppers.h" // stepper motor

#include "bfm_bda.h"

#include "sound/ymz280b.h"
#include "machine/mc68681.h"
#include "machine/nvram.h"
#include "machine/68307.h"
#include "machine/68340.h"
#include "bfm_dm01.h"

#define SC45_BUTTON_MATRIX_1_0 IPT_BUTTON1
#define SC45_BUTTON_MATRIX_1_1 IPT_BUTTON2
#define SC45_BUTTON_MATRIX_1_2 IPT_BUTTON3
#define SC45_BUTTON_MATRIX_1_3 IPT_BUTTON4
#define SC45_BUTTON_MATRIX_1_4 IPT_BUTTON5
#define SC45_BUTTON_MATRIX_1_5 IPT_BUTTON6

#define SC45_BUTTON_MATRIX_2_0 IPT_BUTTON7
#define SC45_BUTTON_MATRIX_2_1 IPT_BUTTON8
#define SC45_BUTTON_MATRIX_2_2 IPT_BUTTON9
#define SC45_BUTTON_MATRIX_2_3 IPT_BUTTON10
#define SC45_BUTTON_MATRIX_2_4 IPT_BUTTON11
#define SC45_BUTTON_MATRIX_2_5 IPT_BUTTON12

#define SC45_BUTTON_MATRIX_7_0 IPT_BUTTON13
#define SC45_BUTTON_MATRIX_7_1 IPT_BUTTON14
#define SC45_BUTTON_MATRIX_7_2 IPT_BUTTON15
#define SC45_BUTTON_MATRIX_7_3 IPT_BUTTON16
#define SC45_BUTTON_MATRIX_7_4 IPT_MAHJONG_A // having more buttons would make sense and be cleaner but as a temporary solution (these are meant to be driven more by the clickable layouts anyway).....
#define SC45_BUTTON_MATRIX_7_5 IPT_MAHJONG_B

#define SC45_BUTTON_MATRIX_8_0 IPT_MAHJONG_C
#define SC45_BUTTON_MATRIX_8_1 IPT_MAHJONG_D
#define SC45_BUTTON_MATRIX_8_2 IPT_MAHJONG_E
#define SC45_BUTTON_MATRIX_8_3 IPT_MAHJONG_F
#define SC45_BUTTON_MATRIX_8_4 IPT_MAHJONG_G
#define SC45_BUTTON_MATRIX_8_5 IPT_MAHJONG_H

#define SC45_BUTTON_MATRIX_9_0 IPT_MAHJONG_I
#define SC45_BUTTON_MATRIX_9_1 IPT_MAHJONG_J
#define SC45_BUTTON_MATRIX_9_2 IPT_MAHJONG_K
#define SC45_BUTTON_MATRIX_9_3 IPT_MAHJONG_L
#define SC45_BUTTON_MATRIX_9_4 IPT_MAHJONG_M
#define SC45_BUTTON_MATRIX_9_5 IPT_MAHJONG_N

#define SC45_BUTTON_MATRIX_10_0 IPT_HANAFUDA_A
#define SC45_BUTTON_MATRIX_10_1 IPT_HANAFUDA_B
#define SC45_BUTTON_MATRIX_10_2 IPT_HANAFUDA_C
#define SC45_BUTTON_MATRIX_10_3 IPT_HANAFUDA_D
#define SC45_BUTTON_MATRIX_10_4 IPT_HANAFUDA_E
#define SC45_BUTTON_MATRIX_10_5 IPT_HANAFUDA_F

#define SC45_BUTTON_MATRIX_20_0 IPT_SERVICE1 // green / test

static const uint8_t SEGMENT_34_ENCODING_LOOKUP[16] =
{
	63, // 0
	6,  // 1
	91, // 2
	79, // 3
	102,// 4
	109,// 5
	125,// 6
	7,  // 7
	127,// 8
	103,// 9
	0,  // 10
	121,// 11
	121,// 12
	121,// 13
	121,// 14
	121,// 15
};

// common base class for things shared between sc4 and sc5
class bfm_sc45_state : public driver_device
{
protected:
	bfm_sc45_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_duart(*this, "duart68681")
		, m_vfd0(*this, "vfd0")
		, m_dm01(*this, "dm01")
		, m_ymz(*this, "ymz")
		, m_lamps(*this, "lamp%u", 0U)
		, m_matrix(*this, "matrix%u", 0U)
		, m_digits(*this, "digit%u", 0U)
	{
	}

	required_device<mc68681_device> m_duart;
	optional_device<bfm_bda_device> m_vfd0;
	optional_device<bfm_dm01_device> m_dm01;
	required_device<ymz280b_device> m_ymz;
	output_finder<0x20 * 8> m_lamps;
	output_finder<0x20 * 8> m_matrix;
	output_finder<32 + 2> m_digits;

	// serial vfd
	int vfd_enabled = 0;
	bool vfd_old_clock = false;

	uint8_t vfd_ser_value = 0U;
	int vfd_ser_count = 0;

	// 34 segment custom encoding used by some sc4/5 machines such as Box Clever, Break The Bank, The Big Deal, The Crazy Chair, The Perfect Game
	bool m_segment_34_encoding = false;
	uint8_t m_segment_34_cache[32]{};

	virtual void machine_start() override;

	void mux_output_w(offs_t offset, uint8_t data);
	void mux_output2_w(offs_t offset, uint8_t data);
	void bfm_sc4_reset_serial_vfd();
	void bfm_sc45_write_serial_vfd(bool cs, bool clock, bool data);
};

void bfm_sc45_write_serial_vfd(running_machine &machine, bool cs, bool clock, bool data);

// sc4 specifics
class sc4_state : public bfm_sc45_state
{
public:
	sc4_state(const machine_config &mconfig, device_type type, const char *tag)
		: bfm_sc45_state(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_cpuregion(*this, "maincpu")
		, m_nvram(*this, "nvram")
		, m_sec(*this, "sec")
		, m_in(*this, "IN-%u", 0U)
		, m_in_coin(*this, "IN-COIN")
		, m_reel(*this, "reel%u", 1U)
		, m_io_ports(*this, "IN-%u", 0)
	{
		m_chk41addr = -1;
		m_dochk41 = false;
	}

	required_device<m68307_cpu_device> m_maincpu;
	required_memory_region m_cpuregion;
	// devices
	required_device<nvram_device> m_nvram;
	required_device<sec_device> m_sec;
	optional_ioport_array<21> m_in;
	required_ioport m_in_coin;
	optional_device_array<stepper_device, 6> m_reel;

	int m_reel12_latch = 0;
	int m_reel3_latch = 0;
	int m_reel4_latch = 0;
	int m_reel56_latch = 0;
	int m_optic_pattern = 0;
	template <unsigned N> DECLARE_WRITE_LINE_MEMBER(reel_optic_cb) { if (state) m_optic_pattern |= (1 << N); else m_optic_pattern &= ~(1 << N); }

	int m_meterstatus = 0;

	int m_chk41addr;
	bool m_dochk41;

	uint16_t m_mainram[0x10000/2]{};

	uint8_t read_input_matrix(int row);


	DECLARE_WRITE_LINE_MEMBER(bfmdm01_busy);

	uint16_t sc4_mem_r(offs_t offset, uint16_t mem_mask = ~0);
	void sc4_mem_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);

	uint16_t sc4_cs1_r(offs_t offset, uint16_t mem_mask = ~0);

	DECLARE_WRITE_LINE_MEMBER(bfm_sc4_duart_irq_handler);
	DECLARE_WRITE_LINE_MEMBER(bfm_sc4_duart_txa);
	uint8_t bfm_sc4_duart_input_r();
	void bfm_sc4_duart_output_w(uint8_t data);

	DECLARE_WRITE_LINE_MEMBER(m68307_duart_txa);
	uint8_t m68307_duart_input_r();
	void m68307_duart_output_w(uint8_t data);

	void init_sc4();
	void init_sc4mbus();
	void init_sc4cvani();
	void init_sc4cvclb();
	void init_sc4bonbxm();
	void init_sc4bonbx();
	void init_sc4crcc();
	void init_sc4fcc();
	void init_sc4luckb();
	void init_sc4luckb_mbus();
	void init_sc4mgr();
	void init_sc4mgrm();
	void init_sc4pgold();
	void init_sc4royle();
	void init_sc4ticlb();
	void init_sc4clbtm();
	void init_sc4bull();
	void init_sc4dnd();
	void init_sc4dndcs();
	void init_sc4dndbb();
	void init_sc4dndcl();
	void init_sc4dnddd();
	void init_sc4dndhf();
	void init_sc4dndys();
	void init_sc4dndlp();
	void init_sc4dndra();
	void init_sc4dndbd();
	void init_sc4dndbr();
	void init_sc4dndcc();
	void init_sc4dnddw();
	void init_sc4dnddf();
	void init_sc4dndpg();
	void init_sc4dndtp();
	void init_sc4dndww();
	void init_sc4dndcw();
	void init_sc4dndtr();
	void init_sc4dndwb();
	void init_sc4dndbe();
	void init_sc4dndbc();
	void init_sc4fguy();
	void init_sc4gd();
	void init_sc4gdclb();
	void init_sc4gbcas();
	void init_sc4gball();
	void init_sc4gunp();
	void init_sc4hapnt();
	void init_sc4hntcsm();
	void init_sc4hntcs();
	void init_sc4hill();
	void init_sc4bwow();
	void init_sc4ggamem();
	void init_sc4ggame();
	void init_sc4gggtb();
	void init_sc4ggg();
	void init_sc4pstat();
	void init_sc4pstat_mbus();
	void init_sc4abra();
	void init_sc4botn();
	void init_sc4broll();
	void init_sc4bigdl();
	void init_sc4bbclb();
	void init_sc4bbclb_mbus();
	void init_sc4blast();
	void init_sc4brix();
	void init_sc4bugs();
	void init_sc4canca();
	void init_sc4captn();
	void init_sc4cmous();
	void init_sc4hotpr();
	void init_sc4cnfr();
	void init_sc4cad();
	void init_sc4cad_mbus();
	void init_sc4cadcl();
	void init_sc4cconx();
	void init_sc4cconxd();
	void init_sc4ccrus();
	void init_sc4chand();
	void init_sc4cinv();
	void init_sc4ckxtb();
	void init_sc4ckx3p();
	void init_sc4cla7();
	void init_sc4disco();
	void init_sc4ghost();
	void init_sc4ghosttb();
	void init_sc4gx();
	void init_sc4gx3();
	void init_sc4rhx();
	void init_sc4rhx_mbus();
	void init_sc4rhxcs();
	void init_sc4rovrt();
	void init_sc4showt();
	void init_sc4swywm();
	void init_sc4sumit();
	void init_sc4swbak();
	void init_sc4bsp();
	void init_sc4clown();
	void init_sc4nunsm();
	void init_sc4acesh();
	void init_sc4pp();
	void init_sc4mrh();
	void init_sc4mhn();
	void init_sc4inspn();
	void init_sc4hiss();
	void init_sc4glad();
	void init_sc4fmj();
	void init_sc4fbspn();
	void init_sc4ddosh();
	void init_sc4ddosh_mbus();
	void init_sc4ctl();
	void init_sc4cmani();
	void init_sc4casxt();
	void init_sc4cari();
	void init_sc4cabin();
	void init_sc4butch();
	void init_sc4bullcs();
	void init_sc4bantm();
	void init_sc4adjb();
	void init_sc4onup();
	void init_sc4blokq();
	void init_sc4xmark();
	void init_sc4wondw();
	void init_sc4wwys();
	void init_sc4wspin();
	void init_sc4tri7tb();
	void init_sc4tri7();
	void init_sc4tri7_mbus();
	void init_sc4tub();
	void init_sc4vrgcl();
	void init_sc4vrgcl_mbus();
	void init_sc4wadzl();
	void init_sc4cleg();
	void init_sc4cleg_mbus();
	void init_sc4leg();
	void init_sc4msclb();
	void init_sc4mspid();
	void init_sc4mspid_mbus();
	void init_sc4maxcc();
	void init_sc4luck7();
	void init_sc4luck7tb();
	void init_sc4ltr2c();
	void init_sc4ltr2c_mbus();
	void init_sc4lotr2();
	void init_sc4lotr2_mbus();
	void init_sc4knok();
	void init_sc4knokb();
	void init_sc4jjc();
	void init_sc4quidv();
	void init_sc4pony();
	void init_sc4popey();
	void init_sc4boomb();
	void init_sc4derby();
	void init_sc4cfgcl();
	void init_sc4cfdu();
	void init_sc4cfcla();
	void init_sc4mmad();
	void init_sc4manic();
	void init_sc4evol();
	void init_sc4emmer();
	void init_sc4dough();
	void init_sc4count();
	void init_sc4ttomb();
	void init_sc4tbana();
	void init_sc4mtb();
	void init_sc4mtbcl();
	void init_sc4monob();
	void init_sc4mont();
	void init_sc4opses();
	void init_sc4nmtj();
	void init_sc4rt();
	void init_sc4rbank();
	void init_sc4sslam();
	void init_sc4sus();
	void init_sc4spice();
	void init_sc4cjcl();
	void init_sc4clclo();
	void init_sc4bobcl();
	void init_sc4bankb();
	void init_sc4bed();
	void init_sc4bedcl();
	void init_sc4bblas();
	void init_sc4bblas_mbus();
	void init_sc4aztec();
	void init_sc4helrd();
	void init_sc4heatw();
	void init_sc4colos();
	void init_sc4fevdt();
	void init_sc4fevnx();
	void init_sc4game();
	void init_sc4goud();
	void init_sc4lasv();
	void init_sc4miljo();
	void init_sc4paytm();
	void init_sc4pglcs();
	void init_sc4redad();
	void init_sc4rvl();
	void init_sc4rvlnx();
	void init_sc4rio();
	void init_sc4strbr();
	void init_sc4twilt();
	void init_sc4monsp();
	void init_sc4ivply();
	void init_sc4ccc();
	void init_sc4valqp();
	void init_sc4winxp();
	void init_sc4xcash();
	void init_sc4helld();
	void init_sc4cashn();
	void init_sc4chain();
	void init_sc4vivam();
	void init_sc4trail();
	void init_sc4typ();
	void init_sc4ttpie();
	void init_sc4rosts();
	void init_sc4redsq();
	void init_sc4pwrbl();
	void init_sc4ppctc();
	void init_sc4potp();
	void init_sc4nmare();
	void init_sc4mr2r();
	void init_sc4mmb();
	void init_sc4mhp();
	void init_sc4mdm();
	void init_sc4mmm();
	void init_sc4bpb();
	void init_sc4ijclb();
	void init_sc4jack();
	void init_sc4jjunc();
	void init_sc4jolly();
	void init_sc4juicy();
	void init_sc4juicy_mbus();
	void init_sc4jjf();
	void init_sc4kalei();
	void init_sc4lir();
	void init_sc4lined();
	void init_sc4lined_mbus();
	void init_sc4ldvcl();
	void init_sc4lockb();
	void init_sc4lkbcl();
	void init_sc4lkbcl_mbus();
	void init_sc4lotrf();
	void init_sc4magic();
	void init_sc4maxim();
	void init_sc4mono5();
	void init_sc4monoa();
	void init_sc4monoa_mbus();
	void init_sc4mondx();
	void init_sc4mou();
	void init_sc4oyf();
	void init_sc4pacqp();
	void init_sc4pacmn();
	void init_sc4paccs();
	void init_sc4paccs_mbus();
	void init_sc4paccl();
	void init_sc4pacpl();
	void init_sc4party();
	void init_sc4pmani();
	void init_sc4ppclb();
	void init_sc4pipe();
	void init_sc4plumb();
	void init_sc4polem();
	void init_sc4polen();
	void init_sc4polic();
	void init_sc4potsh();
	void init_sc4pogbl();
	void init_sc4pwrsg();
	void init_sc4pir();
	void init_sc4pen1();
	void init_sc4quidr();
	void init_sc4qmodo();
	void init_sc4rdrag();
	void init_sc4rdrcl();
	void init_sc4relcz();
	void init_sc4rotc();
	void init_sc4rich();
	void init_sc4rttt();
	void init_sc4rttt_mbus();
	void init_sc4roksc();
	void init_sc4rogds();
	void init_sc4srrmz();
	void init_sc4srrmz_mbus();
	void init_sc4sidsp();
	void init_sc4sirpz();
	void init_sc4smk7();
	void init_sc4srr();
	void init_sc4slc();
	void init_sc4slc_mbus();
	void init_sc4solgl();
	void init_sc4sace();
	void init_sc4sbust();
	void init_sc4starp();
	void init_sc4stirc();
	void init_sc4strk();
	void init_sc4sahed();
	void init_sc4strx();
	void init_sc4suscl();
	void init_sc4suscl_mbus();
	void init_sc4s6c();
	void init_sc4taekw();
	void init_sc4taekw_mbus();
	void init_sc4taknt();
	void init_sc4takcl();
	void init_sc4takcl_mbus();
	void init_sc4tetri();
	void init_sc4tic2();
	void init_sc4tic2_mbus();
	void init_sc4tgear();
	void init_sc4tload();
	void init_sc4tridn();
	void init_sc4trist();
	void init_sc4tristtb();
	void init_sc4valnv();
	void init_sc4vmclb();
	void init_sc4wernr();
	void init_sc4waw();
	void init_sc4winsptb();
	void init_sc4winsp();
	void init_sc4gcclb();
	void init_sc4gcclb_mbus();
	void init_sc4cvclb_mbus();
	void init_sc4spark();
	void init_sc4brkfs();
	void init_sc4gslam();
	void init_sc4cr();
	void init_sc4crcl();
	void init_sc4crcl_mbus();
	void init_sc4cashm();
	void init_sc4chick();
	void init_sc4ccogs();
	void init_sc4cclas();
	void init_sc4cclas_mbus();
	void init_sc4cclim();
	void init_sc4cclim_mbus();
	void init_sc4czfra();
	void init_sc4cfqps();
	void init_sc4cfqps_alt();
	void init_sc4cfcas();
	void init_sc4cfcas_mbus();
	void init_sc4cfclb();
	void init_sc4daylt();
	void init_sc4deepi();
	void init_sc4dblfr();
	void init_sc4darw();
	void init_sc4sstep();
	void init_sc4duckq();
	void init_sc4eascs();
	void init_sc4fastf();
	void init_sc4ffru();
	void init_sc4fpitc();
	void init_sc4fire();
	void init_sc4fwp();
	void init_sc4fd7th();
	void init_sc4frenz();
	void init_sc4ftopi();
	void init_sc4frsu();
	void init_sc4czfr();
	void init_sc4cccsh();
	void init_sc4fullt();
	void init_sc4ziggy();
	void init_sc4gldcl();
	void init_sc4ggrid();
	void init_sc4ggrid_mbus();
	void init_sc4ggcl();
	void init_sc4ggcl_mbus();
	void init_sc4goldo();
	void init_sc4goldw();
	void init_sc4gag();
	void init_sc4greed();
	void init_sc4hf();
	void init_sc4hfcl();
	void init_sc4himi();
	void init_sc4hi5();
	void init_sc4sprng();
	void init_sc4hilo();
	void init_sc4hitsh();
	void init_sc4holyw();
	void init_sc4h6cl();
	void init_sc4jjucl();
	void init_sc4jjucl_mbus();
	void init_sc4monop();
	void init_sc4pglcl();
	void init_sc4po8();
	void init_sc4po8_mbus();
	void init_sc4a40();
	void init_sc4a40cl();
	void init_sc4adwta();
	void init_sc4bar7();
	void init_sc4batl();
	void init_sc4cariq();
	void init_sc4cblas();
	void init_sc4chavi();
	void init_sc4chavy();
	void init_sc4dmine();
	void init_sc4hotsh();
	void init_sc4hotrd();
	void init_sc4hotwd();
	void init_sc4lotrr();
	void init_sc4ldvl();
	void init_sc4ijob();
	void init_sc4vivcs();
	void init_sc4brksp();
	void init_sc4bulcl();
	void init_sc4carry();
	void init_sc4celeb();
	void init_sc4crzky();
	void init_sc4crzky_mbus();
	void init_sc4fever();
	void init_sc4corcl();
	void init_sc4coro();
	void init_sc4corod();
	void init_sc4corotb();
	void init_sc4hyper();

	virtual void machine_start() override;
	virtual void machine_reset() override;

	void bfm_sc4_68307_porta_w(address_space &space, bool dedicated, uint8_t data, uint8_t line_mask);
	void bfm_sc4_reel3_w(uint8_t data);
	void bfm_sc4_reel4_w(uint8_t data);
	void bfm_sc4_68307_portb_w(address_space &space, bool dedicated, uint16_t data, uint16_t line_mask);
	uint8_t bfm_sc4_68307_porta_r(address_space &space, bool dedicated, uint8_t line_mask);
	uint16_t bfm_sc4_68307_portb_r(address_space &space, bool dedicated, uint16_t line_mask);

	void find_mbus(uint16_t* rom);


	void sc4_common(machine_config &config);
	void sc4(machine_config &config);
	void sc4_200_4r(machine_config &config);
	void sc4_200_4ra(machine_config &config);
	void sc4_200_4rb(machine_config &config);
	void sc4_200_5r(machine_config &config);
	void sc4_200_5ra(machine_config &config);
	void sc4_200_5rb(machine_config &config);
	void sc4_200_5rc(machine_config &config);
	void sc4_200_alt(machine_config &config);
	void sc4_200_alta(machine_config &config);
	void sc4_200_altb(machine_config &config);
	void sc4_200_std(machine_config &config);
	void sc4_3reel(machine_config &config);
	void sc4_3reel_200(machine_config &config);
	void sc4_3reel_200_48(machine_config &config);
	void sc4_4reel(machine_config &config);
	void sc4_4reel_200(machine_config &config);
	void sc4_4reel_alt(machine_config &config);
	void sc4_5reel(machine_config &config);
	void sc4_5reel_alt(machine_config &config);
	void sc4_adder4(machine_config &config);
	void sc4_no_reels(machine_config &config);
	void sc4dmd(machine_config &config);
	void sc4_map(address_map &map);

protected:
	optional_ioport_array<16> m_io_ports;
};

class sc4_adder4_state : public sc4_state
{
public:
	sc4_adder4_state(const machine_config &mconfig, device_type type, const char *tag)
		: sc4_state(mconfig, type, tag)
		, m_adder4cpuregion(*this, "adder4")
		, m_adder4cpu(*this, "adder4")
	{ }

	required_region_ptr<uint32_t> m_adder4cpuregion;
	std::unique_ptr<uint32_t[]> m_adder4ram{};

	uint32_t adder4_mem_r(offs_t offset, uint32_t mem_mask = ~0);
	void adder4_mem_w(offs_t offset, uint32_t data, uint32_t mem_mask = ~0);
	virtual void machine_start() override;

	// devices
	required_device<m68340_cpu_device> m_adder4cpu;
	void sc4_adder4(machine_config &config);
	void sc4_adder4_map(address_map &map);
};




INPUT_PORTS_EXTERN( sc4_base );
INPUT_PORTS_EXTERN( sc4_raw );

#define SC4_JACKPOT_KEY_SETTINGS \
	PORT_DIPSETTING(    0x00, "No Key" ) \
	PORT_DIPSETTING(    0x01, "75GBP (club)" ) \
	PORT_DIPSETTING(    0x02, "100GBP (club)" ) \
	PORT_DIPSETTING(    0x03, "250GBP (club)" ) \
	PORT_DIPSETTING(    0x04, "150GBP (club)" ) \
	PORT_DIPSETTING(    0x05, "5" ) \
	PORT_DIPSETTING(    0x06, "6" ) \
	PORT_DIPSETTING(    0x07, "500GBP (club)" ) \
	PORT_DIPSETTING(    0x08, "5GBP" ) \
	PORT_DIPSETTING(    0x09, "15GBP" ) \
	PORT_DIPSETTING(    0x0a, "25GBP (4GBP, Maz)" ) \
	PORT_DIPSETTING(    0x0b, "11" ) \
	PORT_DIPSETTING(    0x0c, "35GBP" ) \
	PORT_DIPSETTING(    0x0d, "70GBP" ) \
	PORT_DIPSETTING(    0x0e, "14" ) \
	PORT_DIPSETTING(    0x0f, "15" )


#define SC4_STANDARD_STAKE_KEY \
	PORT_MODIFY("IN-3") /* the Stake key */ \
	PORT_DIPNAME( 0x1c, 0x00, "IN 3-2:4 (Stake Key)" ) \
	PORT_DIPSETTING(    0x00, "0" ) \
	PORT_DIPSETTING(    0x04, "1" ) \
	PORT_DIPSETTING(    0x08, "2" ) \
	PORT_DIPSETTING(    0x0c, "3" ) \
	PORT_DIPSETTING(    0x10, "4" ) \
	PORT_DIPSETTING(    0x14, "5" ) \
	PORT_DIPSETTING(    0x18, "6" ) \
	PORT_DIPSETTING(    0x1c, "7" )

#define SC4_STANDARD_SYSBUTS \
	PORT_MODIFY("IN-20") \
	PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_SERVICE1) PORT_NAME("Green Button (Service?)") /* next to the dips on the MB */ \
	PORT_DIPNAME( 0x0002, 0x0000, "Door Lock" ) /* DORLOK20.1 */ \
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) ) \
	PORT_DIPSETTING(      0x0002, DEF_STR( On ) ) \
	PORT_DIPNAME( 0x0004, 0x0000, "Service Door" ) /* SERDOR20.2 */ \
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) ) \
	PORT_DIPSETTING(      0x0004, DEF_STR( On ) ) \
	PORT_DIPNAME( 0x0008, 0x0000, "Cashbox Door" ) /* CSHDOR20.3 */ \
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) ) \
	PORT_DIPSETTING(      0x0008, DEF_STR( On ) ) \
	PORT_DIPNAME( 0x0010, 0x0000, "Hopper DMP" ) /* HOPDMP20.4 */ \
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) ) \
	PORT_DIPSETTING(      0x0010, DEF_STR( On ) )


#define SC4_STANDARD_PERCENT_KEY \
	PORT_MODIFY("IN-6") /* the %age key */ \
	PORT_DIPNAME( 0x0f, 0x00, "IN 6-0:3 (Percentage Key)" ) \
	PORT_DIPSETTING(    0x00, "0" ) \
	PORT_DIPSETTING(    0x01, "1" ) \
	PORT_DIPSETTING(    0x02, "2" ) \
	PORT_DIPSETTING(    0x03, "3" ) \
	PORT_DIPSETTING(    0x04, "4" ) \
	PORT_DIPSETTING(    0x05, "5" ) \
	PORT_DIPSETTING(    0x06, "6" ) \
	PORT_DIPSETTING(    0x07, "7" ) \
	PORT_DIPSETTING(    0x08, "8" ) \
	PORT_DIPSETTING(    0x09, "9" ) \
	PORT_DIPSETTING(    0x0a, "a" ) \
	PORT_DIPSETTING(    0x0b, "b" ) \
	PORT_DIPSETTING(    0x0c, "c" ) \
	PORT_DIPSETTING(    0x0d, "d" ) \
	PORT_DIPSETTING(    0x0e, "e" ) \
	PORT_DIPSETTING(    0x0f, "f" )

#define SC4_STANDARD_BLANK_DIPS \
	PORT_MODIFY("IN-16") \
	PORT_DIPNAME( 0x0001, 0x0000, "IN 16-0 (DSW1)"  ) /* DIL1 16.0 */ \
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) ) \
	PORT_DIPSETTING(      0x0001, DEF_STR( On ) ) \
	PORT_DIPNAME( 0x0002, 0x0000, "IN 16-1 (DSW2)" ) /* DIL2 16.1 */ \
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) ) \
	PORT_DIPSETTING(      0x0002, DEF_STR( On ) ) \
	PORT_DIPNAME( 0x0004, 0x0000, "IN 16-2 (DSW3)" ) /* DIL3 16.2 */ \
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) ) \
	PORT_DIPSETTING(      0x0004, DEF_STR( On ) ) \
	PORT_DIPNAME( 0x0008, 0x0000, "IN 16-3 (DSW4)" ) /* DIL4 16.3 */ \
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) ) \
	PORT_DIPSETTING(      0x0008, DEF_STR( On ) ) \
	PORT_DIPNAME( 0x0010, 0x0000, "IN 16-4 (DSW5)" ) /* DIL5 16.4 */ \
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) ) \
	PORT_DIPSETTING(      0x0010, DEF_STR( On ) ) \
	PORT_MODIFY("IN-17") \
	PORT_DIPNAME( 0x0001, 0x0000, "IN 17-0 (DSW6)" ) /* DIL6 17.0 */ \
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) ) \
	PORT_DIPSETTING(      0x0001, DEF_STR( On ) ) \
	PORT_DIPNAME( 0x0002, 0x0000, "IN 17-1 (DSW7)" ) /* DIL7 17.1 */ \
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) ) \
	PORT_DIPSETTING(      0x0002, DEF_STR( On ) ) \
	PORT_DIPNAME( 0x0004, 0x0000, "IN 17-2 (DSW8)" ) /* DIL8 17.2 */ \
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) ) \
	PORT_DIPSETTING(      0x0004, DEF_STR( On ) ) \
	PORT_DIPNAME( 0x0008, 0x0000, "IN 17-3 (DSW9)" ) /* DIL9 17.3 */ \
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) ) \
	PORT_DIPSETTING(      0x0008, DEF_STR( On ) ) \
	PORT_DIPNAME( 0x0010, 0x0000, "IN 17-4 (DSW10)" ) /* DIL10 17.4 */ \
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) ) \
	PORT_DIPSETTING(      0x0010, DEF_STR( On ) ) \
	PORT_MODIFY("IN-18") \
	PORT_DIPNAME( 0x0001, 0x0000, "IN 18-0 (DSW11)" ) /* DIL11 18.0  */ \
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) ) \
	PORT_DIPSETTING(      0x0001, DEF_STR( On ) ) \
	PORT_DIPNAME( 0x0002, 0x0000, "IN 18-1 (DSW12)" ) /* DIL12 18.1 */ \
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) ) \
	PORT_DIPSETTING(      0x0002, DEF_STR( On ) ) \
	PORT_DIPNAME( 0x0004, 0x0000, "IN 18-2 (DSW13)" ) /* DIL13 18.2 */ \
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) ) \
	PORT_DIPSETTING(      0x0004, DEF_STR( On ) ) \
	PORT_DIPNAME( 0x0008, 0x0000, "IN 18-3 (DSW14)" ) /* DIL14 18.3 */ \
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) ) \
	PORT_DIPSETTING(      0x0008, DEF_STR( On ) ) \
	PORT_DIPNAME( 0x0010, 0x0000, "IN 18-4 (DSW15)" ) /* DIL15 18.4 */ \
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) ) \
	PORT_DIPSETTING(      0x0010, DEF_STR( On ) ) \
	PORT_MODIFY("IN-19") \
	PORT_DIPNAME( 0x0001, 0x0000, "IN 19-0 (DSW16)" ) /* DIL9 19.0 */ \
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) ) \
	PORT_DIPSETTING(      0x0001, DEF_STR( On ) )



#define sc_ivply_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* 95004313 4PLAY PR1227 */ \
	ROM_LOAD( "95004313.snd", 0x0000, 0x080000, CRC(28faed9e) SHA1(624d6cfcc97dd1950b5d908cde2e4d2eb6ea1dc6) )
#define sc_plays_others \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95891106.bin", 0x0000, 0x5000, CRC(1f616820) SHA1(c0b19d4243f250c3159564c65e9a2247e61e315f) ) \
	ROM_LOAD( "95891107.bin", 0x0000, 0x5000, CRC(6af72c14) SHA1(4666e338fc629a32178f617c8536e7f11d703b14) ) \
	ROM_LOAD( "95891108.bin", 0x0000, 0x5000, CRC(f44de048) SHA1(783ae2b12e8e548a90f626a2050968e3f38b6042) ) \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* TEDL SOUNDS */ \
	ROM_LOAD( "95009013.bin", 0x0000, 0x0f3ee2, CRC(f324a590) SHA1(0533af3304100c99af5fdeba2773d92384101f43) )
#define sc_a40_others \
	ROM_REGION( 0x1000000, "ymz", ROMREGION_ERASE00 ) /* AROU SOUNDS */ \
	ROM_LOAD( "95008511.bin", 0x0000, 0x0ff5c3, CRC(6fac2014) SHA1(5b364406f7b5d8f4c54561e3977571e4a47273a0) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890123.bin", 0x0000, 0x5000, CRC(e3f5466e) SHA1(ae2f45f79d0113e651b8cf5f655a9ceb32a3ddaa) ) \
	ROM_LOAD( "95890124.bin", 0x0000, 0x5000, CRC(9663025a) SHA1(097246412fd4084c1185d8dfd91e884abc5a3f8a) ) \
	ROM_LOAD( "95890125.bin", 0x0000, 0x5000, CRC(08d9ce06) SHA1(9087170d6c6edad735ab5a2abbc0a2191aabfd03) )
#define sc_a40cl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* AROU SOUNDS */ \
	ROM_LOAD( "95008527.bin", 0x0000, 0x0ff5c3, CRC(6fac2014) SHA1(5b364406f7b5d8f4c54561e3977571e4a47273a0) )
#define sc_acesh_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* ACEHI SOUNDS */ \
	ROM_LOAD( "95008292.bin", 0x0000, 0x100000, CRC(1c238098) SHA1(e2e2cb52ea84a7dc5e557b61b3ee21bd5a063833) )
#define sc_adjb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR3009 JIGGY BANK SOUNDS11 */ \
	ROM_LOAD( "95008699.bin", 0x000000, 0x100000, CRC(6d49b54a) SHA1(d64caa1dae40c3bf536f8739d6995b589c7184b7) ) \
	ROM_LOAD( "95008700.bin", 0x100000, 0x100000, CRC(49c37b2a) SHA1(5a59a540becb8c83288ae3a15cfac069fd792e74) )


#define sc_adsnt_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR1611 ANT AND DECS SOUNDS11 */ \
	ROM_LOAD( "95008478.bin", 0x000000, 0x100000, CRC(d8aa3daf) SHA1(dafeb014f04179882921622df9b5c9917279bd02) ) \
	ROM_LOAD( "95008479.bin", 0x100000, 0x100000, CRC(f1ec9928) SHA1(cd2eb8115217c205f248564c9340353c84db2ac5) )

#define sc_adwta_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR1940 ADSNT SHOWTIME SOUNDS11 */ \
	ROM_LOAD( "95008656.bin", 0x000000, 0x100000, CRC(632a4cbb) SHA1(455e6242f5c5c1a0d84aadfdda33873e8e7b2418) ) \
	ROM_LOAD( "95008657.bin", 0x100000, 0x100000, CRC(aa68db50) SHA1(52ee314549fac989dd9ee494144ee3bbb99ee283) )
#define sc_aztec_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* 95004286 AZTEC PR1155 */ \
	ROM_LOAD( "95004286.snd", 0x0000, 0x080000, CRC(20de4ebe) SHA1(0ea12d0c46f90efd8b040c2374d749fb7b15698e) )
#define sc_bjob_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR000053 BANKJOB 1 */ \
	ROM_LOAD( "95009126.bin", 0x000000, 0x100000, CRC(55940c44) SHA1(e4f2f36ccf28446011e43f5c8a72fa9d15f72df0) ) \
	ROM_LOAD( "95009127.bin", 0x100000, 0x100000, CRC(0d3ee6d9) SHA1(a40bffaf631010d53ce9228758bbf5dba8423f6e) )

#define sc_bucc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR3216 BUCKANEERS SOUNDS11 */ \
	ROM_LOAD( "95008806.bin", 0x000000, 0x100000, CRC(16ee0143) SHA1(0890da3b2108ec67db1cc8ed2dc40f2eac2a5fbc) ) \
	ROM_LOAD( "95008807.bin", 0x100000, 0x100000, CRC(7141737f) SHA1(74607352f32df2a27f0724f243532c98ca315910) )
#define sc_bunny_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* BUNN SOUNDS */ \
	ROM_LOAD( "95008907.bin", 0x0000, 0x08b8c3, CRC(2b4911fd) SHA1(7ccdb5e5accb548c10ef288fd901825505559866) )
#define sc_bantm_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* BANTAM SOUNDS */ \
	ROM_LOAD( "95008962.bin", 0x000000, 0x100000, CRC(3c10480f) SHA1(67c86228b6199e336beaa21b2982ce3cf5867ed1) ) \
	ROM_LOAD( "95008963.bin", 0x100000, 0x100000, CRC(8ceb0bf8) SHA1(b5712f145ff515c10b0a3b131d0c215a31adbb42) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890434.bin", 0x0000, 0x5000, CRC(37740955) SHA1(c7a9839a4f7f01c916b696a8baedf33d0c146fa3) ) \
	ROM_LOAD( "95890435.bin", 0x0000, 0x5000, CRC(42e24d61) SHA1(dd915ccf979d61cdb0ff7598d35cdceb43d6d8e9) ) \
	ROM_LOAD( "95890436.bin", 0x0000, 0x5000, CRC(dc58813d) SHA1(9bad2c6cdacaa016f5239e578600f1ff03f1ed63) )
#define sc_bar7_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR1433 BAR SEVENS SOUNDS11 */ \
	ROM_LOAD( "95008409.bin", 0x0000, 0x080000, CRC(ec29c758) SHA1(ceb99fb6edcab5c5e1b0ec46d622ee0f2cdb20be) )
#define PR1492_CAS_BAR_SEVENS_SOUNDS11 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR1492 CAS BAR SEVENS SOUNDS11 */ \
	ROM_LOAD( "95008502.bin", 0x0000, 0x080000, CRC(d9bc2c75) SHA1(b9e637d47287a844a6ff1f0b1d1a34b48a806aad) )
#define sc_batl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008541.bin", 0x000000, 0x100000, CRC(a739cba8) SHA1(b968cfa18c671aaf027909961a843fc01101d68a) ) \
	ROM_LOAD( "95008542.bin", 0x100000, 0x100000, CRC(686bb7cc) SHA1(4e809f2d1401030127d7afa853189c4fca9742e5) )
#define sc_bedcl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008397.bin", 0x0000, 0x0fe1b3, CRC(d59cd40d) SHA1(585e42b66e691ab9a3df5bb7dae4368226861b04) )
#define sc_bblas_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008302", 0x0000, 0x0fea01, CRC(ef880aee) SHA1(00177f6a0bfe2006d8545834ea4fb22427be97c4) )
#define sc_bbust_others \
	ROM_REGION( 0x100000, "ymz", 0 ) \
	ROM_LOAD( "95008050.bin", 0x0000, 0x0fc8a1, CRC(8f1a49b5) SHA1(9d75e7118c628b3665a31376a3e35797b7058f6b) )


#define sc_bobcl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008967.bin", 0x0000, 0x0f64c6, CRC(cf1e1c59) SHA1(f7ccbc6ed20aed329c2a199a489686bb8e87edfb) )
#define sc_bonbx_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008992.bin", 0x0000, 0x0aa536, CRC(aad10089) SHA1(d8a32f66432ee901be05435e8930d3897f4b4e33) ) /* BARX - right header, wrong sounds? */ \
	ROM_LOAD( "95009015.bin", 0x0000, 0x0b7821, CRC(8b6ca362) SHA1(4c50935b6d1038738ce631fbdc359416197b8c03) ) /* BARX - right header, wrong sounds? */
#define sc_brksp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008210.bin", 0x0000, 0x1aea2f, CRC(da15a50e) SHA1(2f068526992358789e5f2c347a5adf56a1bfde52) )


#define sc_bpfpb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008771.bin", 0x000000, 0x100000, CRC(121b6db2) SHA1(634f435635fcb08013729629fc8f56a4180c2034) ) \
	ROM_LOAD( "95008772.bin", 0x100000, 0x100000, CRC(c8813870) SHA1(5c4571a3ca9a06c49e80c5e10e9fd288de560324) )

#define sc_bpb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008553.bin", 0x000000, 0x100000, CRC(f8019fb2) SHA1(aa5baed73538ebc6e39c6e35437b77a5cc22135e) ) \
	ROM_LOAD( "95008554.bin", 0x100000, 0x100000, CRC(8e00a011) SHA1(821686d96ee6fb45ab0b771934ce25ab7a230c0c) )
#define sc_bull_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008520.bin", 0x000000, 0x100000, CRC(ab418ce0) SHA1(39afbd449a21a31016acaffcfb4f6d684db300e7) ) \
	ROM_LOAD( "95008521.bin", 0x100000, 0x100000, CRC(7fd0f70c) SHA1(e6bf85b9e2528dac5811277ee89599e58e6358b0) )
#define PR1743_CASINO_BULLSEYE_SOUNDS11 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008544.bin", 0x000000, 0x100000, CRC(19777f20) SHA1(da0cc08b00f9ab565a0e559761adbf7bcd7c327b) )
#define PR1758_BULLSEYE_TRIPLE_SOUNDS11 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008575.bin", 0x000000, 0x100000, CRC(715901d2) SHA1(a7d3d0c7b9fc3eefb0df372cdf0cae7abe8bd959) )
#define PR3318_BULLSEYE_SOUNDS11 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009116.bin", 0x000000, 0x100000, CRC(5a01e206) SHA1(38ee3706d9edb68d17c90bb627aa17b4f908d721) ) \
	ROM_LOAD( "95009117.bin", 0x100000, 0x100000, CRC(c3623829) SHA1(92b0cf89678023bb02394699e2e214389317ec6c) )


#define sc_butch_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008604.bin", 0x000000, 0x100000, CRC(dede59e0) SHA1(de56b80d0a53fbb18acce2b907f48801e8fced12) ) \
	ROM_LOAD( "95008605.bin", 0x100000, 0x100000, CRC(3db1a818) SHA1(2c20d39dc0d7eb3996209a0b34afae3cd2eebbdc) )

#define sc_cfact_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008508.bin", 0x000000, 0x100000, CRC(97d26d77) SHA1(828e3797184b39e1b8fd788253071f323f2d890e) ) \
	ROM_LOAD( "95008509.bin", 0x100000, 0x100000, CRC(8d1e1f36) SHA1(8976a74c050158fc63fbcbfbadd3f41fca187a24) )
#define sc_ctit_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008762.bin", 0x000000, 0x100000, CRC(cf7fc030) SHA1(0c34d9b35eb6229ae48b27f2f88ff682b4601816) ) \
	ROM_LOAD( "95008763.bin", 0x100000, 0x100000, CRC(767b94d9) SHA1(5cac6831e54af03eea065143c04bda54f44fec7d) )
#define sc_craid_others \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890715.bin", 0x0000, 0x5000, CRC(60af654b) SHA1(ec714992a41b154f534aac0a515e1ef490dad0db) ) \
	ROM_LOAD( "95890716.bin", 0x0000, 0x5000, CRC(1539217f) SHA1(bc766636048c720ac5df55bbf643e2801536e58f) ) \
	ROM_LOAD( "95890717.bin", 0x0000, 0x5000, CRC(8b83ed23) SHA1(43d56d728fbbb81c9e660e537746cd2aa01aadd1) ) \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008851.bin", 0x0000, 0x0fa2fc, CRC(ec82c860) SHA1(7806065db24c13208c77009c64de91a3fb0f4311) )
#define sc_cvega_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008477.bin", 0x0000, 0x100000, CRC(74607f2f) SHA1(8ce25e6af2fa8b2b4acd794dd78511983d10f7f5) )
#define sc_chopc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008908.bin", 0x0000, 0x0fa18a, CRC(78e8e1cf) SHA1(0211fffcaab36d88610525c85efb5ea7835fb641) )
#define sc_cbar7_others \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95891086.bin", 0x0000, 0x5000, CRC(64066892) SHA1(acdf17a6fec87c3094b4b363d5f5a21d58e6260a) ) \
	ROM_LOAD( "95891087.bin", 0x0000, 0x5000, CRC(11902ca6) SHA1(2c7fbf5ec8f22cf1d29d7eb437b8e0403ebbfb13) ) \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009002.bin", 0x0000, 0x05dab2, CRC(7763eeea) SHA1(946ec95a75e3ffdba98c67d58162f7d2e456480e) )
#define sc_clu70_others \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95891124.bin", 0x0000, 0x5000, CRC(efb25e7e) SHA1(f1da25d6f6c2fe9953c0f95e889b845e65a1fe27) ) \
	ROM_LOAD( "95891125.bin", 0x0000, 0x5000, CRC(9a241a4a) SHA1(3383c31a1de5f4a0ced5e3346de7b7e1f7fdbe04) ) \
	ROM_LOAD( "95891126.bin", 0x0000, 0x5000, CRC(049ed616) SHA1(2c29271dfebff92b3f537dd3cc89aea53decf978) ) \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009017.bin", 0x000000, 0x100000, CRC(4df62a63) SHA1(1d3b7927683d45bf81c038527b57881a58657e21) ) \
	ROM_LOAD( "95009018.bin", 0x100000, 0x030022, CRC(90c95f34) SHA1(a63d52b7ec202f6757ed24251e542be2409ba2a9) )
#define sc_clcas_others \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890486.bin", 0x0000, 0x5000, CRC(52bab0b5) SHA1(2144c1c9b980f6dbc1948e94c34fc2213fc5aa70) ) \
	ROM_LOAD( "95890487.bin", 0x0000, 0x5000, CRC(4e6c4009) SHA1(44ae2ce42ff9b91174e214213ccf52c281827434) ) \
	ROM_LOAD( "95890488.bin", 0x0000, 0x5000, CRC(8d366299) SHA1(1d0cd59dfb39ca3a78ae2398992a992d34b2e61a) ) \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008985.bin", 0x0000, 0x0b57f9, CRC(63366697) SHA1(cdab5d0aa1bf738e4e326ed31f78f2a797bd8bd6) )
#define sc_clnot_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009135.bin", 0x000000, 0x100000, CRC(af699e46) SHA1(e8f685847f4eb83a67074cae92ed59e96f37653b) ) \
	ROM_LOAD( "95009136.bin", 0x100000, 0x100000, CRC(cd1ea94c) SHA1(8010ba1490c656f870230611d15255db175a5a49) )
#define sc_clus_others \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890495.bin", 0x0000, 0x5000, CRC(a475b60f) SHA1(4f02f0e0c19be7378736d6d4707f7b4a3068be9b) ) \
	ROM_LOAD( "95890496.bin", 0x0000, 0x5000, CRC(d1e3f23b) SHA1(6f9032c000bcc0cd61180193d85d0a4623026d14) ) \
	ROM_LOAD( "95890497.bin", 0x0000, 0x5000, CRC(4f593e67) SHA1(3a4591ae8ca140dff54d5e74143359fbecc14bfc) ) \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008988.bin", 0x0000, 0x0d23ea, CRC(b33581a0) SHA1(667bf4e499b50fd072313bad671ffcbd299fd97b) )
#define PR3502_C_OF_M_TEN_SOUNDS11 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009031.bin", 0x000000, 0x100000, CRC(58ded502) SHA1(27c05614b03b51fe4a168b8c64535ec3ca9e85c1) ) \
	ROM_LOAD( "95009032.bin", 0x100000, 0x100000, CRC(e1eb711a) SHA1(f6c0c10992f2aacd6f9fadc4e15d8ad5ba5b4ed8) )
#define PR3317_COLOUR_OF_MONEY_SOUNDS11 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009124.bin", 0x000000, 0x100000, CRC(798d8d5a) SHA1(931bfca5d204c62fc834959d9babcd9488fc7f07) ) \
	ROM_LOAD( "95009125.bin", 0x100000, 0x100000, CRC(d6090812) SHA1(16ca4868725eff0cc4bd056d751dc7861c1e53a1) )
#define sc_cfnc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009178.bin", 0x000000, 0x100000, CRC(eb938e0a) SHA1(978fa5554462d5d2cbf6a696c6c754318e950223) ) \
	ROM_LOAD( "95009179.bin", 0x100000, 0x100000, CRC(baf8b8b7) SHA1(43b967f3909270b99b563dab4bd1035079a5cb78) )
#define sc_cfcp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009035.bin", 0x000000, 0x100000, CRC(8138c442) SHA1(ceaeb84a27d0e3eba8d3a9a7c76e86b4214178c9) ) \
	ROM_LOAD( "95009036.bin", 0x100000, 0x100000, CRC(757f44b9) SHA1(61fe15024f7c7f97f865b1b666a5f837a57446a5) )
#define sc_cfpt_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR1359 CRAZY F PARTY SOUNDS11 */ \
	ROM_LOAD( "95008348.bin", 0x000000, 0x100000, CRC(9c6db3d1) SHA1(f2f613326d0a8c0a1c2e1eea1e91ed34cd344f41) ) \
	ROM_REGION( 0x400000, "others1", ROMREGION_ERASE00 ) /* PR1359 TB CRAZY F P SOUNDS11 (topbox rom?)*/ \
	ROM_LOAD( "95008349.bin", 0x100000, 0x100000, CRC(6f7716a0) SHA1(75f8fbee605b96e36e8596068dd4570928a5a02d) ) \
	ROM_REGION( 0x400000, "others2", ROMREGION_ERASE00 ) /* PR1359 TB CRAZY F P SOUNDS11 (alt topbox rom?) */ \
	ROM_LOAD( "95008363.bin", 0x000000, 0x100000, CRC(6d211d7b) SHA1(59dbb6201aa355a585253d17302e4f8ceed9a220) )
#define sc_cknig_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009193.bin", 0x000000, 0x100000, CRC(6c3cb54b) SHA1(a140209d19f9cbd18ede08924b94718264ba4118) ) \
	ROM_LOAD( "95009194.bin", 0x100000, 0x100000, CRC(44229686) SHA1(eda0481282614e6bd82985486ef4eea0eb0ddccd) )
#define sc_cpays_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008528.bin", 0x0000, 0x100000, CRC(eccde2dc) SHA1(83fc283269fea98ea1b2c0b846bb790bf84d3053) )
#define sc_canca \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004270.bin", 0x0000, 0x0d4379, CRC(c0db303d) SHA1(8929e4901d517e6063dc75ff40cbcd478bb0be8a) )
#define sc_carry_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008240.bin", 0x0000, 0x100000, CRC(7c693111) SHA1(6807d78d32e817babd4e1ed2c0b7313d7cc8ebc2) )
#define sc_cmous_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008439.bin", 0x0000, 0x0e94e0, CRC(9bcded46) SHA1(9f1b431ac010f2ab9a8d393b167ff81baf7f86a5) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890395.bin", 0x0000, 0x5000, CRC(31480d43) SHA1(9ac6254b539d57f79fbb2483948db393ca4823eb) ) \
	ROM_LOAD( "95890396.bin", 0x0000, 0x5000, CRC(2d9efdff) SHA1(0b6642bb202092eb1ef8791a3a1f2b79ad1b5fff) ) \
	ROM_LOAD( "95890397.bin", 0x0000, 0x5000, CRC(eec4df6f) SHA1(b4fd028f28fa43d54c6aaf6062ba79a37f232025) ) \
	ROM_REGION( 0x400000, "others", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008947.bin", 0x0000, 0x0fc44b, CRC(457a86c3) SHA1(e36ba57a981bb0f8b581c4b8e1ddd6e55c24bf00) ) /* cash & mouse s16 sounds */

#define sc_cnfr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004130.lo", 0x000000, 0x080000, CRC(223e2bd4) SHA1(f8e187cc74e524496c9e5b35260ac56b93e6e83b) ) \
	ROM_LOAD( "95004131.hi", 0x080000, 0x080000, CRC(3840ffd6) SHA1(d72901b08c11d1f5b16fdb1a8fe57625cc25bd44) )
#define sc_cad_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008396.bin", 0x0000, 0x0ff641, CRC(f24c5ef6) SHA1(83395f64b79efbd4f3153e5d0a2d2b0d0183f857) )
#define sc_cadcl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008486.bin", 0x0000, 0x0ff641, CRC(05e8cba2) SHA1(7a99ab3f45588cff0e54a94ffd75df337cff7351) )

#define sc_clash_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008374.bin", 0x0000, 0x0feb46, CRC(47ea13e2) SHA1(f9b6e23b3857da5f1f364469c8d4deb4f8d836a6) )
#define sc_cr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008245.bin", 0x0000, 0x0ffadf, CRC(6528bb5c) SHA1(d54e1f0a18dbbc91316d57be25e0cf5236e856e6) )
#define sc_crcl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008338.bin", 0x0000, 0x0ffadf, CRC(b26edbfe) SHA1(9f5c7a8e6781bad6448f767d19474bbad8024df5) )
#define sc_cckey_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008425.bin", 0x0000, 0x100000, CRC(99bf2289) SHA1(596fe427dd7abf2779e12e3f20a5ba2eda553f2e) )
#define sc_chub_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004109", 0x000000, 0x080000, CRC(2e3e2f6e) SHA1(217197bdbcd27271afce7809de7c49cca4ef3656) ) \
	ROM_LOAD( "95004110", 0x080000, 0x080000, CRC(29f3caef) SHA1(dddf38b3da4b9756103b32de4d5d12d612223298) )
#define sc_cla7_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004334.bin", 0x0000, 0x080000, CRC(bd8a3d25) SHA1(9dd5d124ce467293417575bf316dc1b371bc3169) )
#define sc_clclo_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008889.bin", 0x0000, 0x0edfec, CRC(fb894b4f) SHA1(60b3f28efda4ecb9f7804540291c8c3b8bead27a) )

#define sc_colos_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004235.sn1", 0x000000, 0x080000, CRC(e543f4ab) SHA1(7d72f1392fbd43afe390d4f3059b06cd789528d4) ) \
	ROM_LOAD( "95004236.sn2", 0x080000, 0x080000, CRC(0f26a957) SHA1(b0949534d44dd22b6f676203f52fad978b36d04b) )




#define sc_cccsh_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004173", 0x000000, 0x080000, CRC(67970fed) SHA1(938414921a0956841ce4447471b7663f5c2c2d4f) ) \
	ROM_LOAD( "95004174", 0x080000, 0x080000, CRC(1f1dd01f) SHA1(6ed312c1e50056bf3cae156e595b26374d7d4816) )
#define sc_ckx_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008995.bin", 0x0000, 0x0931b3, CRC(e874bd15) SHA1(8adb32532dd053ca2c4e0987352013310ce197a0) )
#define sc_chick_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008291.bin", 0x0000, 0x0b2877, CRC(8e9f53d4) SHA1(7c91e64d7214e1c08b0976f901713228bb64ee49) )
#define sc_cabin_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008902.bin", 0x0000, 0x0e7fd3, CRC(c7b0f675) SHA1(e19d4e65d73329a58b9cbc6828c7e0c287f2906a) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890306.bin", 0x0000, 0x5000, CRC(eff2e8d8) SHA1(0749d543bf760b2037330ade671c7ce7ed64b275) ) \
	ROM_LOAD( "95890307.bin", 0x0000, 0x5000, CRC(9a64acec) SHA1(51c49e8d415d99e61fda99ade43bb925d99f3825) ) \
	ROM_LOAD( "95890308.bin", 0x0000, 0x5000, CRC(04de60b0) SHA1(cee280d9528f9e167be1c373e7a99f1f2f1bc1c0) )
#define sc_cari_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )  /* CABC SOUNDS */ \
	ROM_LOAD( "95008981.bin", 0x000000, 0x0ff245, CRC(3a888f98) SHA1(b113c17acda287200fdcd734a1b384879a5e7101) ) \
	ROM_REGION( 0x5000, "pics", 0 ) /* do these belong to this, or the QPS game? */ \
	ROM_LOAD( "95890474.bin", 0x0000, 0x5000, CRC(bcb54114) SHA1(0236171b34f3e47db9aa253f9605ff6bd21b1460) ) \
	ROM_LOAD( "95890475.bin", 0x0000, 0x5000, CRC(c9230520) SHA1(5cf9c3d130f114dac633d696e0bf1bda94afb9ee) ) \
	ROM_LOAD( "95890476.bin", 0x0000, 0x5000, CRC(5799c97c) SHA1(5dab858e61815c04f874f231848c376d8fce1c3d) )
#define sc_cariq_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* CARIBBEANSND */ \
	ROM_LOAD( "97000070.bin", 0x000000, 0x0f8526, CRC(27ba6768) SHA1(708e836f3fc35fabd0a5c8dc9cd8e9327d7bcdc7) ) \
	ROM_REGION( 0x400000, "others1", ROMREGION_ERASE00 ) /* CARIBBEANSND (same data?) */ \
	ROM_LOAD( "caribbeancashsnd1.bin", 0x000000, 0x080000, CRC(89da9fc4) SHA1(d56364e2e71d03d7d8888966b64ff1fab4bfc3e9) ) \
	ROM_LOAD( "caribbeancashsnd2.bin", 0x080000, 0x080000, CRC(99d33f61) SHA1(7caf3b9540372900e90a7141f14383fe06936a2e) )
#define sc_cblas_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008539.bin", 0x0000, 0x0e0033, CRC(068349f4) SHA1(b27cba3217122397eb7aea0d9a382fda9ee6a43b) )

#define sc_casxt_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008854.bin", 0x0000, 0x08cba2, CRC(f7b3ad12) SHA1(08108745f7e048e9aa27777bdc6edeb5d5836a6d) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890718.bin", 0x0000, 0x5000, CRC(dd65993f) SHA1(db2af897e36d7b55ed1d1d2efe350f4802449945) ) \
	ROM_LOAD( "95890719.bin", 0x0000, 0x5000, CRC(c1b36983) SHA1(2b74336915c7156723e2554f152ed34f6c88a16a) ) \
	ROM_LOAD( "95890720.bin", 0x0000, 0x5000, CRC(02e94b13) SHA1(2d8fffc631e1a590774131bdea1f0b5ec8c6e8f3) )

#define sc_cmani_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008295.bin", 0x0000, 0x100000, CRC(a5646d3b) SHA1(9aca69f103f959a85026fd0d0ffb3bd65eebae34) )
#define sc_cj_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008427.bin", 0x0000, 0x100000, CRC(a6093378) SHA1(30a64e8857da269cbc0b3febf30a62f5647d10b9) )
#define sc_ctl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008225.bin", 0x0000, 0x100000, CRC(b185224e) SHA1(a4b4c618903c4c207de8dae74b55fd2c85a5e4ff) )
#define sc_crsc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008681.bin", 0x0000, 0x100000, CRC(7a459110) SHA1(51f0a51af485a0f4e447ee0c22f488a924e45117) )
#define sc_cfqps_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR6813 CRAZY FRUITS SOUNDS11 */ \
	ROM_LOAD( "95004101.bin", 0x000000, 0x080000, CRC(31e5785a) SHA1(8da2bd3571640b0ae23e9fdcc2235dcf9d2de122) ) \
	ROM_LOAD( "95004102.bin", 0x080000, 0x080000, CRC(bf059b21) SHA1(89a96c593e5385e9b40f694768986691eed4b5c5) ) \
	ROM_REGION( 0x400000, "others", ROMREGION_ERASE00 ) /* PR6813 CRAZY FRUITS SOUNDS11 (same data?) */ \
	ROM_LOAD( "95008339.bin", 0x000000, 0x100000, CRC(bae07fa8) SHA1(a86a5d9ff6ac802bd15275f46894374317627e23) )
#define sc_czfr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR6813 CRAZY FRUITS SOUNDS11 */ \
	ROM_LOAD( "crazy sound 1.bin", 0x000000, 0x080000, CRC(31e5785a) SHA1(8da2bd3571640b0ae23e9fdcc2235dcf9d2de122) ) \
	ROM_LOAD( "crazy sound 2.bin", 0x080000, 0x080000, CRC(bf059b21) SHA1(89a96c593e5385e9b40f694768986691eed4b5c5) ) \
	\
	ROM_REGION( 0x400000, "others", ROMREGION_ERASE00 ) \
	ROM_LOAD( "oki_sound_rom--ccfsnd.bin", 0x0000, 0x02196a, CRC(15e542c2) SHA1(9bd9218b7392c882f68e330619bea7996afd001e) ) /* oki sound rom, doubt it belongs here */
#define SOUND_95004150_CRAZY_FRUITS_PR6982 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004150.bin", 0x000000, 0x080000, CRC(7707ea0d) SHA1(efe187a4c64a3a5f8d10e21c269afbb603186ae7) ) \
	ROM_LOAD( "95004151.bin", 0x080000, 0x080000, CRC(54845756) SHA1(e53dfa8f836044232ee1a8006051ef586ac24d8c) )
#define PR1152_CRAZY_FRUITS_SOUNDS11 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004280.sn1", 0x000000, 0x080000, CRC(44d1b0d9) SHA1(2e361a1b65955acecbc26ba47000e4ef6316a22d) ) \
	ROM_LOAD( "95004281.sn2", 0x080000, 0x080000, CRC(31f057f3) SHA1(a6415eee8499ada43225901dee2d12c93d494658) )

#define sc_crnjw_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008469.bin", 0x0000, 0x100000, CRC(07df5a41) SHA1(56dacba05e26258e50085f78dfe965196917addf) ) /* PR1608 CROWN JEWELS SOUNDS11 */
#define PR000055_CJEWELS \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009176.bin", 0x000000, 0x100000, CRC(d1db2564) SHA1(641f94108df9dffde599012018d944e3848f50ed) ) \
	ROM_LOAD( "95009177.bin", 0x100000, 0x100000, CRC(e574cc1c) SHA1(760284d60d336571495da921ff666ac5725d7c19) )
#define sc_cclim_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008243.bin", 0x0000, 0x100000, CRC(b21e073b) SHA1(dfe309490c5c69052041c2121258c17b974f88ed) )
#define sc_chavi_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008563.bin", 0x0000, 0x100000, CRC(e4d30521) SHA1(70a2d88842ee40c77011a2ed8ca45f71316d2579) )
#define sc_chavy_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008964.bin", 0x0000, 0x0fc821, CRC(597bc1c8) SHA1(f28d3fe1dd5ccf4d5ff6e1088a392b4713e6d1b5) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890437.bin", 0x0000, 0x5000, CRC(e4c29dae) SHA1(5e81acde46a489b8a9056167e33efec3d2d1b95a) ) \
	ROM_LOAD( "95890438.bin", 0x0000, 0x5000, CRC(9154d99a) SHA1(61ef137e6db5f8394b948c589c0cee506a48f9ed) ) \
	ROM_LOAD( "95890439.bin", 0x0000, 0x5000, CRC(0fee15c6) SHA1(3d24e9861c2c7a67a3d4d156b9281e857de30a55) )

#define sc_crotr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009049.bin", 0x000000, 0x100000, CRC(10c94788) SHA1(8fb5113036a0122a4dbc9af15b600ffe99e51094) ) \
	ROM_LOAD( "95009050.bin", 0x100000, 0x100000, CRC(5e47e86c) SHA1(9aaca1390852f9079cd355c48b889fb5962c5c09) )

#define sc_crsgr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009139.bin", 0x000000, 0x100000, CRC(f2e4fa3c) SHA1(1909de32f232e5bfb3ee3c624f29c585cca254c2) ) \
	ROM_LOAD( "95009140.bin", 0x100000, 0x100000, CRC(81bdf47f) SHA1(49c02fb87a4ffc1be4c748c469f5518c314b1391) )
#define sc_copsr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR1412 */ \
	ROM_LOAD( "95008393.bin", 0x0000, 0x100000, CRC(693cd9be) SHA1(e965b2dcd6d6619815c919b7d2c372c726abfea3) ) \
	ROM_REGION( 0x400000, "others1", ROMREGION_ERASE00 ) /* PR3206 */ \
	ROM_LOAD( "95008787.bin", 0x0000, 0x100000, CRC(82713d8b) SHA1(317cb1c482bd13e1cad2e5739c99f6e3ca163684) ) \
	ROM_REGION( 0x400000, "others2", ROMREGION_ERASE00 ) /* PR3255 */ \
	ROM_LOAD( "95008846.bin", 0x0000, 0x080000, CRC(44f9eaab) SHA1(394bd36121c33aba97bc957b3d46d2944c1daeed) ) \
	ROM_REGION( 0x400000, "others3", ROMREGION_ERASE00 ) /* PR3255 (alt checksum, many more sounds) */ \
	ROM_LOAD( "95009130.bin", 0x0000, 0x100000, CRC(f6813371) SHA1(bd9e5d009c7870d50149d9de396f363fb19b183c) ) \
	ROM_REGION( 0x400000, "others4", ROMREGION_ERASE00 ) /* bad? no header */ \
	ROM_LOAD( "cops_rob_snd.bin", 0x0000, 0x080000, CRC(e25bddb7) SHA1(96d6be2407bf876ad685bdc13d96585782ad4a50) )

#define sc_corst_others \
	/* PR3521 CORRONATION ST SOUNDS11 */ \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009037.bin", 0x000000, 0x100000, CRC(c2d3b6ee) SHA1(36e7e9c956ab91fd1b2e52800b4482ce3a3597f5) ) \
	ROM_LOAD( "95009038.bin", 0x100000, 0x100000, CRC(45dc4df3) SHA1(7710a0ba9bbe7ae6f590b61bbc1395cd408926d2) )
#define sc_coro_others \
	/* CORRY SOUNDS */ \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008939.bin", 0x000000, 0x100000, CRC(02c9a21d) SHA1(7f27a8d706a7d66feafc5faab23ee28ebfa02b53) ) \
	ROM_LOAD( "95008940.bin", 0x100000, 0x05fdb4, CRC(09c8a19a) SHA1(21a58f8183c29d61c0132c2fc7339e459521a16e) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890365.bin", 0x0000, 0x5000, CRC(12f90bc7) SHA1(09cdfddfa7fb430eccd87d677673f203120bef51) ) \
	ROM_LOAD( "95890366.bin", 0x0000, 0x5000, CRC(676f4ff3) SHA1(c6a3c2b01b79e9abaa249b548fb59584c63fc682) ) \
	ROM_LOAD( "95890367.bin", 0x0000, 0x5000, CRC(f9d583af) SHA1(f22e3ed7c4d954a98cec9dc077113761fdcef858) ) \
	ROM_LOAD( "95890377.bin", 0x0000, 0x5000, CRC(edbf2523) SHA1(8165c2c2f12fd85f706aed31bc3cbd51648440de) ) \
	ROM_LOAD( "95890378.bin", 0x0000, 0x5000, CRC(f169d59f) SHA1(fa8116f43c25de7af05f1cf8a379a24cce489f21) ) \
	ROM_LOAD( "95890379.bin", 0x0000, 0x5000, CRC(3233f70f) SHA1(df485c4df31872921c3d97973a7b3e7d4a58446d) )
#define sc_corcl_others \
	/* CCORO SOUNDS */ \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008867.bin", 0x000000, 0x100000, CRC(b11779d2) SHA1(720d30445232baa18e613259bded1a6827777de6) ) \
	ROM_LOAD( "95008868.bin", 0x100000, 0x05fdb4, CRC(424b08e5) SHA1(fc0fe01516c0c6bb20adcf2bf8b1263ff59657e8) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890770.bin", 0x0000, 0x5000, CRC(551ab279) SHA1(39e4c16f3cc708d84762ba5b80c720cbbc123f0d) ) /* why is this in an sc4 set? */

#define sc_count_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008615.bin", 0x000000, 0x100000, CRC(97c602c0) SHA1(cd71c062df3ac700ab72fc992542e08651270ec8) ) \
	ROM_LOAD( "95008616.bin", 0x100000, 0x100000, CRC(2b80b1a5) SHA1(a94b4402ba738a6369f58c73a5b9d66e0476eeb1) )

#define sc_devil_others \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890464.bin", 0x0000, 0x5000, CRC(35e24491) SHA1(e762854ac074df67db843fed4c6c7cfe1d8f754b) ) \
	ROM_LOAD( "95890465.bin", 0x0000, 0x5000, CRC(407400a5) SHA1(dba6f4204a27f26056ffc3d9b512c0f4a8f17562) ) \
	ROM_LOAD( "95890466.bin", 0x0000, 0x5000, CRC(dececcf9) SHA1(6ebaec58330b7ce8bb166d2aefd203147c4823fa) ) \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008978.bin", 0x0000, 0x0ff462, CRC(b0dac89b) SHA1(afdc4fec96849b81e52aa03d9df821e336001c74) )
#define sc_daylt_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "daylsnd1.bin", 0x000000, 0x080000, CRC(4510d156) SHA1(b001bdf3bfad6124647f640065b411c07c8cf555) ) \
	ROM_LOAD( "daylsnd2.bin", 0x080000, 0x080000, CRC(71a1428f) SHA1(029ae979f192bcd7704ae78aba14815acdabe60f) )
#define sc_derby_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008129.bin", 0x000000, 0x100000, CRC(4892cf07) SHA1(9ce745f4b3aee623a7855e2fee4cc35de20646a3) ) \
	ROM_LOAD( "95008130.bin", 0x100000, 0x100000, CRC(b9bfde2a) SHA1(25329d5b93318586095715111fbbb854bba394b9) )
#define sc_deepi_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008329.bin", 0x0000, 0x100000, CRC(427dad17) SHA1(f8834dfbe6608abb08d6214cd87ec809df963805) )
#define sc_ddosh_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008299.bin", 0x0000, 0x100000, CRC(fee81425) SHA1(188cb7275963193a39571d3e4da3b9f6693a79d0) )
#define sc_dhh_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008586.bin", 0x000000, 0x100000, CRC(43fde3ad) SHA1(662402449b025bbf75a10c3b1ac746d9359b0b0d) ) \
	ROM_LOAD( "95008587.bin", 0x100000, 0x100000, CRC(6f7a607f) SHA1(da3c8aaa69b30f10f122a9bde49d987dd0950474) )
#define sc_dmine_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008293.bin", 0x0000, 0x100000, CRC(acbed8e1) SHA1(607dafa2215632d02c34e65b79bf1341b981f63b) )
/* PR1954 */

#define PR1954_CASINO_DOND_R_SOUNDS11 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008631.bin", 0x000000, 0x100000, CRC(7208854a) SHA1(a02de60cfcbafe5da4f67596ab65237f5b5f41b7) )
/* PR1945 */

#define PR1945_CASINO_DOND_R_SOUNDS12 \
	ROM_LOAD( "95008661.bin", 0x000000, 0x100000, CRC(2d9ebcd5) SHA1(d824a227420cbe616aca6e2fd279af691ddfd87a) )
#define PR1945_CASINO_D_O_N_D_SOUNDS11 \
	ROM_LOAD( "95008632.bin", 0x000000, 0x100000, CRC(fd81a317) SHA1(1e597fd58aab5a7a8321dc4daf138ee07b42c094) ) \
	ROM_LOAD( "95008633.bin", 0x100000, 0x100000, CRC(a7a445d4) SHA1(dbb1938c33ce654c2d4aa3b6af8c210f5aad2ae3) )
/* PR1964 */

#define PR1964_CASINO_DOND_SOUNDS11 \
	ROM_LOAD( "95008680.bin", 0x000000, 0x100000, CRC(9bd439d1) SHA1(5e71d04e5697e92998bae28f7352ea7742cafe07) )
#define PR1964_CASINO_DOND_SOUNDS12 \
	ROM_LOAD( "95008698.bin", 0x000000, 0x100000, CRC(8eea7754) SHA1(7612c128d6c062bba3477d55aee3089e1255f61e) )
#define PR1964_DND \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	PR1964_CASINO_DOND_SOUNDS11 \
	PR1964_CASINO_DOND_SOUNDS12
/* PR1907 */

#define PR1907_DEAL_OR_NO_DEAL_SOUNDS11 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008606.bin", 0x000000, 0x100000, CRC(c63c8663) SHA1(4718baf87340fe93ccfe642a1a0cdb9d9dcac57f) ) \
	ROM_LOAD( "95008607.bin", 0x100000, 0x100000, CRC(a615514f) SHA1(d4ff7d4fe6f1dd1d7b00cc504f90b2921aa5e8fb) )


#define sc_dndbb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008788.bin", 0x000000, 0x100000, CRC(51384d64) SHA1(3af87bcc7fb4881ae671d9cbb484fb6a2a534cce) ) \
	ROM_LOAD( "95008789.bin", 0x100000, 0x100000, CRC(4f2d72c3) SHA1(f167ad8d7178682833e7e9efcc393b5826108d70) )
#define sc_dndbo_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009158.bin", 0x000000, 0x100000, CRC(27f11b81) SHA1(f2690c2828ad09d73cb14c5c1cb53c2ad1fc7b53) ) \
	ROM_LOAD( "95009159.bin", 0x100000, 0x100000, CRC(95e278c8) SHA1(7d85b6dbe3ccacd6c5a5b4eb475598a3d5548ce0) )

#define sc_dndbq_others \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95891057.bin", 0x0000, 0x5000, CRC(75c0cf91) SHA1(20e7fb3a279cc127cec08a78b6633852b0a950f7) ) \
	ROM_LOAD( "95891058.bin", 0x0000, 0x5000, CRC(69163f2d) SHA1(66b6b593126c452f01b36c03e82d003860340025) ) \
	ROM_LOAD( "95891059.bin", 0x0000, 0x5000, CRC(aa4c1dbd) SHA1(4ebd1b132a4125bdcf58d5867cffb821e3e7b1a3) ) \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008887.bin", 0x0000, 0x100000, CRC(53cb9dc5) SHA1(66cdf8624736ea5a4e239bdb81fc96366ec4623e) )
#define PR3068_BEAT_THE_BANKER_SOUNDS11 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008713.bin", 0x000000, 0x100000, CRC(7311db5e) SHA1(24a4b3f44fa6dbbe756b0acac2b513c6e92cdf26) ) \
	ROM_LOAD( "95008714.bin", 0x100000, 0x100000, CRC(c6ebc845) SHA1(6a135f42940b8c44422736650e369fa12b51e0df) )

#define sc_dndcl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008629.bin", 0x000000, 0x100000, CRC(6f7cf500) SHA1(bf3db728872251116b32aa2c44f72a87d29b13d2) ) \
	ROM_LOAD( "95008630.bin", 0x100000, 0x100000, CRC(5dfda41e) SHA1(6d61d0eedc6a1599777f47d3880689f954f326c9) )
#define sc_dndcr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009033.bin", 0x000000, 0x100000, CRC(4e167844) SHA1(d0e7101fe033bc9fc0e279c3d457a7da8e8aab00) ) \
	ROM_LOAD( "95009034.bin", 0x100000, 0x100000, CRC(a3e7b40a) SHA1(09f1fde693750f15e78da91c796e8593d06d19e9) )
#define sc_dnddd_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* dond double */ \
	ROM_LOAD( "95008804.bin", 0x000000, 0x100000, CRC(19fd19fa) SHA1(db68a40d8bb3de130c6d5777217fb3c3a4c13d17) ) \
	ROM_LOAD( "95008805.bin", 0x100000, 0x100000, CRC(f497337d) SHA1(9e827116f8b1d882cfcd0b56aaede9de14b2e943) )
#define sc_dndde_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009053.bin", 0x000000, 0x100000, CRC(f06acb47) SHA1(043756eda04d193f7365f2fd0fafc36c3b6b0bd9) ) \
	ROM_LOAD( "95009054.bin", 0x100000, 0x100000, CRC(595baae8) SHA1(9aa03b265294eaed0051b0a25cc9a0e1481b4390) )
#define sc_dnddi_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009191.bin", 0x000000, 0x100000, CRC(2f3cda1a) SHA1(017533caadf86d0d70e1b57b697621e0b4a8fef2) ) \
	ROM_LOAD( "95009192.bin", 0x100000, 0x100000, CRC(eb9b6b0b) SHA1(127bb283b32a997c04a1b29c18f4093f4d969151) )
#define sc_dnddt_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009106.bin", 0x000000, 0x100000, CRC(afa266cf) SHA1(adc80bc677a7524bec4bbd102a4a5ff293571abc) ) \
	ROM_LOAD( "95009107.bin", 0x100000, 0x100000, CRC(493981a8) SHA1(77308e453848d092d63330e10e3b1e88caeb4fb5) )
#define sc_dnddo_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009120.bin", 0x000000, 0x100000, CRC(327c01a0) SHA1(9b7fa28ff6ecc83c2e9ee73aac0af98be170b28f) ) \
	ROM_LOAD( "95009121.bin", 0x100000, 0x100000, CRC(d7991c7e) SHA1(4f4e4043f272ff2325b0fa64c66b34cb1aefb97c) )
#define sc_dndfl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008847.bin", 0x000000, 0x100000, CRC(7f3eae56) SHA1(f5d25c87caf76fc1961c8fb5c389bb73cefbcb28) ) \
	ROM_LOAD( "95008848.bin", 0x100000, 0x100000, CRC(ba00c16d) SHA1(f2e55441857e5f036e19d2ee29dfad97b99134b8) )
#define sc_dndgo_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009197.bin", 0x000000, 0x100000, CRC(bdf253cb) SHA1(6f046f1c27bae1141919874b27a8d87295b39261) ) \
	ROM_LOAD( "95009198.bin", 0x100000, 0x100000, CRC(420105d4) SHA1(23e4eac4146a3985c3a7dc5f80f1b5900a116303) )
#define sc_dndgl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009102.bin", 0x000000, 0x100000, CRC(9d03cd95) SHA1(e062c816a19af0dc86bfa5a66f69342e69fffcfa) ) \
	ROM_LOAD( "95009103.bin", 0x100000, 0x100000, CRC(51ae4095) SHA1(b397984d3b0beb4ca95050ff595648a80e5ea87a) )
#define sc_dndld_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009184.bin", 0x000000, 0x100000, CRC(25a36077) SHA1(92c05f410456414b825b09338a474c52f7c1bd9b) ) \
	ROM_LOAD( "95009185.bin", 0x100000, 0x100000, CRC(eacdfc26) SHA1(8ab462e7f49ca9374b9245d9d97cf909ea979ae3) )
#define sc_dndmb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009156.bin", 0x000000, 0x100000, CRC(7c68c70c) SHA1(eb4006e185f3cd4e7d66999b3ff064a7903f5bc8) ) \
	ROM_LOAD( "95009157.bin", 0x100000, 0x100000, CRC(a0541504) SHA1(689ba32fd1b1a9592a9cd7039424655a652b1bcd) )
#define sc_dndmd_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009170.bin", 0x000000, 0x100000, CRC(6ab363a5) SHA1(1a98566161aec187074cbbdb7372ed8484202639) ) \
	ROM_LOAD( "95009171.bin", 0x100000, 0x100000, CRC(bc5d3113) SHA1(836eff78265f6d0371be18467c5edc6d32aba6d5) )
#define sc_dndpa_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009150.bin", 0x000000, 0x100000, CRC(54d9f01a) SHA1(6d62ca07443c42e16243a843dee6a11c7a1383fc) ) \
	ROM_LOAD( "95009151.bin", 0x100000, 0x100000, CRC(89bb7543) SHA1(e8bea8a00798329af590be5e2ccbfcc54f70f98d) )
#define sc_ddptg_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009041.bin", 0x000000, 0x100000, CRC(143d05fb) SHA1(f3d2735dc5b1bdc4e3f4205521f1b614d4b4c4b0) ) \
	ROM_LOAD( "95009042.bin", 0x100000, 0x100000, CRC(65efd8fd) SHA1(c7f2d1f6b31767a56269f53e9327de3abe64218f) )
#define sc_ddply_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008830.bin", 0x000000, 0x100000, CRC(bb66ae0c) SHA1(4a7df180dd67aa96059a83986cb98721a81963d8) ) \
	ROM_LOAD( "95008831.bin", 0x100000, 0x100000, CRC(8a632940) SHA1(e955b05a82312ab356117f348903292d6f26a5a5) )
#define sc_dndrr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009188.bin", 0x000000, 0x100000, CRC(a2dceadd) SHA1(af1696bf32f37005c6a01bb209f38b90daab4e29) ) \
	ROM_LOAD( "95009189.bin", 0x100000, 0x100000, CRC(02b0d336) SHA1(7713212ebf59fa2464110b0238a870e81d43be05) )
#define sc_dndsi_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008838.bin", 0x000000, 0x100000, CRC(34e9d2eb) SHA1(140f9ccf287e24e3917b1a6bb8d8118bf20f232c) ) \
	ROM_LOAD( "95008839.bin", 0x100000, 0x100000, CRC(4475bb6b) SHA1(864575f9b35495bfe0d8f7563e2d4b5e35b6a396) )
#define sc_dndrt_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009141.bin", 0x000000, 0x100000, CRC(e53f8ba3) SHA1(10eea3246352bf65e1ca7b4cd6e1a873d6a2f7fe) ) \
	ROM_LOAD( "95009142.bin", 0x100000, 0x100000, CRC(54b47cbd) SHA1(71e8f49b0d031fcc8c5450279dc26d39b90af678) )
#define sc_dndpd_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009172.bin", 0x000000, 0x100000, CRC(f124785f) SHA1(2291dca731bb6037d655dc8bbdfa7f35f6a4d4d1) ) \
	ROM_LOAD( "95009173.bin", 0x100000, 0x100000, CRC(aae9318c) SHA1(4b53bec7284950fbca4650117e727ab9a01b5509) )

#define sc_dndtb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009186.bin", 0x000000, 0x100000, CRC(1da5489b) SHA1(b282e523b52e34a625502ec99ca42f059e701448) ) \
	ROM_LOAD( "95009187.bin", 0x100000, 0x100000, CRC(2c3ea82c) SHA1(50ade918503e224288407abf60b3d53e5334aadb) )
#define sc_dndhf_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008705.bin", 0x000000, 0x100000, CRC(09e02843) SHA1(a6ac658c857eca2aca0bacda423dd012434e93bc) ) \
	ROM_LOAD( "95008706.bin", 0x100000, 0x100000, CRC(805e7eb8) SHA1(107c6ba26e37d8f2eec81b62c3edf3efb7e44ca2) )
#define sc_dndys_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008779.bin", 0x000000, 0x100000, CRC(cf04d115) SHA1(3255b58cd4fba7d231ca35c00d1cb70da1f6cfbd) ) \
	ROM_LOAD( "95008780.bin", 0x100000, 0x100000, CRC(ec270dbe) SHA1(f649ffd4530feed491dc050f40aa0205f4bfdd89) )
#define sc_dndww_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008688.bin", 0x000000, 0x100000, CRC(9a5c8ac0) SHA1(988a26b042acd1462927c1b8ad300fe67c8d7c00) ) \
	ROM_LOAD( "95008689.bin", 0x100000, 0x100000, CRC(5c1cf5fb) SHA1(c05b819158622ab96c265c85072b48ae3593b962) ) \
	ROM_REGION( 0x400000, "others", ROMREGION_ERASE00 ) /* casino set */ \
	ROM_LOAD( "95008730.bin", 0x0000, 0x100000, CRC(e2aebdb0) SHA1(d22fee7ff3d5912ea9a7440ec82de52a7d016090) ) /* casino wow */
#define sc_dndtr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008832.bin", 0x000000, 0x100000, CRC(1d2aa204) SHA1(52ad7a6ea369198b48d593de508f317ed11e84de) ) \
	ROM_LOAD( "95008833.bin", 0x100000, 0x100000, CRC(92f462f6) SHA1(c0782639d4cd2653a3d14e1b422c2d6117dac029) )
#define sc_dndwb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR1962 */ \
	ROM_LOAD( "95008651.bin", 0x000000, 0x100000, CRC(c3738d54) SHA1(bf3c62e196a2cea144868729f136000a7b924c3d) ) /* wiyb */ \
	ROM_LOAD( "95008652.bin", 0x100000, 0x100000, CRC(ba8f5b62) SHA1(2683623e8b90d5e9586295d720aa1d985b416117) )  \
	ROM_REGION( 0x400000, "others1", ROMREGION_ERASE00 ) /* PR3071 */ \
	ROM_LOAD( "95008715.bin", 0x000000, 0x100000, CRC(1796b604) SHA1(9045f9424e6447d696a6fdd7f5bdcbfda4b57c90) ) /* wiyb */ \
	ROM_REGION( 0x400000, "others2", ROMREGION_ERASE00 ) /* PR3071 (alt checksum, extra sound??) */ \
	ROM_LOAD( "95008766.bin", 0x000000, 0x100000, CRC(62accb81) SHA1(ed0456cefed2fbf9a1cfd911f871262e892a4ce8) ) /* wiyb */ \
	ROM_REGION( 0x400000, "others3", ROMREGION_ERASE00 ) /* PR3084 (wrong game?) */ \
	ROM_LOAD( "95008767.bin", 0x000000, 0x100000, CRC(0aa49d8a) SHA1(a2baffc495656f0b5426b720cac298a78774b7fa) ) /* super slam?? */
#define sc_dndpg_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008822.bin", 0x000000, 0x100000, CRC(16411f05) SHA1(11f2d17b6647b9d9e713e38bab6604e4feb51432) ) \
	ROM_LOAD( "95008823.bin", 0x100000, 0x100000, CRC(4f7bab89) SHA1(bbb203cff3212571c313e8f22f9083980baf2566) )
#define sc_dndtp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008673.bin", 0x000000, 0x100000, CRC(4b8d1e0b) SHA1(8cba9632abb2800f8e9a45d1bf0abbc9abe8cb8c) ) \
	ROM_LOAD( "95008674.bin", 0x100000, 0x100000, CRC(66236e20) SHA1(222fefdeddb22eb290302528a8f937468ccd5698) )

#define sc_dndbe_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008624.bin", 0x000000, 0x100000, CRC(bf9620ea) SHA1(63f5a209da3d0117fcb579364a53b23d2b02cfe5) ) \
	ROM_LOAD( "95008625.bin", 0x100000, 0x100000, CRC(2e1a1db0) SHA1(41ebad0615d0ad3fea6f2c00e2bb170d5e417e4a) )
#define sc_dndbc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009100.bin", 0x000000, 0x100000, CRC(b06110c0) SHA1(84289721a8c71124cc4df79fc90d0ded8d43fd07) ) \
	ROM_LOAD( "95009101.bin", 0x100000, 0x100000, CRC(53b38d0a) SHA1(1da40cbee8a18713864e3a578ac49c2108585e44) )
#define sc_dndlp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008792.bin", 0x000000, 0x100000, CRC(2e7e1a5a) SHA1(000cb48c67b7f23d00318d37206c2df426c79281) ) \
	ROM_LOAD( "95008793.bin", 0x100000, 0x100000, CRC(4b91b638) SHA1(b97cb1d22f91a791fc4d47bbae8965882898a317) )
#define sc_dndra_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008752.bin", 0x000000, 0x100000, CRC(82bfd510) SHA1(857f294d46d64275b15c56187bbbc19e2aa0f5bc) ) \
	ROM_LOAD( "95008753.bin", 0x100000, 0x100000, CRC(ee0b5da9) SHA1(f6cb6037f525df504c1ba8106f19990ecf8d1bd2) )
#define sc_dndbd_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008790.bin", 0x000000, 0x100000, CRC(3c56a8b6) SHA1(8c06aa725233f4feb7c2e703f203cf9b0c6669a1) ) \
	ROM_LOAD( "95008791.bin", 0x100000, 0x100000, CRC(7378a4a8) SHA1(af9dd1fcbd8b77fab4afc85a325c0eb011f35e3a) )
#define sc_dndbr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008723.bin", 0x000000, 0x100000, CRC(6e1d9bd5) SHA1(92b2fd0b75a195af6ddfbdee3316f8af4bc4eb1a) ) \
	ROM_LOAD( "95008724.bin", 0x100000, 0x100000, CRC(309c1bf1) SHA1(82811b503a06e30a915eebdbdbcd63b567f241c1) )
#define sc_dndcc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008746.bin", 0x000000, 0x100000, CRC(ef3c39b9) SHA1(21b9788080de68acd436608d7d46fd9663d1589b) ) /* crazy chair */ \
	ROM_LOAD( "95008747.bin", 0x100000, 0x100000, CRC(9f9b638f) SHA1(5eae49f8a9571f8fade9acf0fc80ea3d70cc9e18) ) \
	\
	ROM_REGION( 0x400000, "others", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008840.bin", 0x000000, 0x100000, CRC(2aa17d67) SHA1(43bdee1b3d0a0bf28f672620867fa3dc95727fbc) ) /* crazy chair */ \
	ROM_LOAD( "95008841.bin", 0x100000, 0x100000, CRC(594a9cd7) SHA1(667fa3106327ce4ff23877f2d48d3e3e360848d0) ) \
	\
	ROM_LOAD( "95009131.bin", 0x000000, 0x100000, CRC(59a9e50a) SHA1(15d79b177a1e926cd5aee0f969f5ef8b30fac203) ) /* crazy chair */ \
	ROM_LOAD( "95009132.bin", 0x100000, 0x100000, CRC(5e8a7ca6) SHA1(419ecc6ac15004bdd83bcd5b3e00d9dcd0d24936) )
#define sc_dnddw_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008754.bin", 0x000000, 0x100000, CRC(d2c3e3e5) SHA1(3d21c812456618471a331d596760ea8746afc036) ) \
	ROM_LOAD( "95008755.bin", 0x100000, 0x100000, CRC(e6906180) SHA1(81215fd3dfe315123d5b028047a93e30baa52b5d) )
#define sc_dnddf_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008738.bin", 0x000000, 0x100000, CRC(2a06f488) SHA1(666bd9c8091edda4b003fa586e5ae270831c119f) ) \
	ROM_LOAD( "95008739.bin", 0x100000, 0x100000, CRC(33d677b0) SHA1(5283d23671e340e3fda4eb4c6795d1d56b073206) )
#define sc_disco_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004220", 0x000000, 0x080000, CRC(530be6ac) SHA1(eede25d9d8a429dace91c1dc37ce4da7152b1836) ) \
	ROM_LOAD( "95004221", 0x080000, 0x080000, CRC(0b2020c9) SHA1(75c7af9312ea1b84df66a640fac903aedac5a0eb) )
#define sc_dbldm_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008540.bin", 0x0000, 0x0ff77d, CRC(752628b0) SHA1(42b07736549b0e5e9d15cccd4922fac1e4135687) )
#define sc_dblfr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004307.bin", 0x0000, 0x080000, CRC(69b55ce9) SHA1(c635add8001dc7c5c81ffcae05b2b83bb37e9e72) )
#define sc_dough_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008505.bin", 0x0000, 0x100000, CRC(e18549be) SHA1(7062f05b6245c4188ed68a51dc7fbc8b138698f4) )

#define sc_emmer_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008973.bin", 0x0000, 0x0fc2ce, CRC(a6ef4f4c) SHA1(4749e008e8cd0a7f92da85fcbf1a9dfd9fd5e721) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890452.bin", 0x0000, 0x5000, CRC(193927ce) SHA1(fe02d1d1519139199fbb743ce0e67b810261e9a2) ) \
	ROM_LOAD( "95890453.bin", 0x0000, 0x5000, CRC(6caf63fa) SHA1(86657e0e14eac91f91bd4da4c04b88c8ee28de45) ) \
	ROM_LOAD( "95890454.bin", 0x0000, 0x5000, CRC(f215afa6) SHA1(0e28affa6557474856087dbeeb0a3127616b7fe6) )
#define sc_evol_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "evolsnd.p1", 0x000000, 0x100000, CRC(13e18cc3) SHA1(114301af1abf86cdd9c2f01b316d257bb1ca086d) ) \
	ROM_LOAD( "evolsnd.p2", 0x100000, 0x08399a, CRC(345f3f11) SHA1(dd4d4f0f32650304cc6010b8f059c23dd5535919) )
#define sc_fgbh_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009118.bin", 0x000000, 0x100000, CRC(f6ce47b9) SHA1(bb3917a3eed718e23dc37b1ffe93f2e349990d3c) ) \
	ROM_LOAD( "95009119.bin", 0x100000, 0x100000, CRC(9d96e3fd) SHA1(5d2d3939a2f1f765c2272f7747a0eea7d0cf1bf8) )
#define sc_fggp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008824.bin", 0x000000, 0x100000, CRC(58119c7f) SHA1(40f6376b5e1caf0c3231a9b870f584e9b2362676) ) \
	ROM_LOAD( "95008825.bin", 0x100000, 0x100000, CRC(7fdae5e2) SHA1(57bcb01b66bdd7ad68dc601f67bc13229ad604f8) )
#define sc_fast_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008453.bin", 0x0000, 0x0a5c12, CRC(bc1d0d4b) SHA1(1a6de68ead3287b32741dd596ca23b64e2a47925) )
#define sc_fnclb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008670.bin", 0x000000, 0x100000, CRC(345c0993) SHA1(9fdc586c415c5f4586b8395d4a8d93d87792d43e) ) \
	ROM_LOAD( "95008671.bin", 0x100000, 0x100000, CRC(11dd3c77) SHA1(f125c510a10819dac3391a582277f99b53c131fa) )

#define sc_fires_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008442.bin", 0x0000, 0x100000, CRC(8b815735) SHA1(8374c706a5378bb8923fd331b96dd07f0204b2e7) )
#define sc_ftcas_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008894.bin", 0x0000, 0x0ff163, CRC(9a25809f) SHA1(8be16ac75056be025b4881d50493650c8e290183) )
#define sc_floop_others \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890455.bin", 0x0000, 0x5000, CRC(e69028e8) SHA1(4b2f2daf0f0cda3fb9c48fe04eaedce3a1bb1bcd) ) \
	ROM_LOAD( "95890456.bin", 0x0000, 0x5000, CRC(93066cdc) SHA1(6bae40c87d083d11ec3b920637c10427ab30d0e2) ) \
	ROM_LOAD( "95890457.bin", 0x0000, 0x5000, CRC(0dbca080) SHA1(3855987a78f1110741ed8580999c3eceb9126378) ) \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008974.bin", 0x0000, 0x0fdd10, CRC(22b0f002) SHA1(df0e66b693ea5587b6cb54847adafc4720dddaa6) )
#define sc_fbspn_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008526.bin", 0x0000, 0x100000, CRC(81890751) SHA1(1e9cedc07c9028eb65620371d9fcf73ae84ad274) )
#define sc_fire_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008307.bin", 0x0000, 0x100000, CRC(e1b9b37c) SHA1(96bc9cb530f94b77c3243b2d9d743aeed209d64d) )
#define sc_fguy_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008813.bin", 0x000000, 0x100000, CRC(e5548d68) SHA1(ac20af71cedd0c6d61d6a28afa8d700c1446e442) ) \
	ROM_LOAD( "95008814.bin", 0x100000, 0x100000, CRC(06f3165c) SHA1(936a8c1d31bca93a5b985e89c48a1faaef602ae0) )
#define sc_fmj_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008982.bin", 0x0000, 0x0fe74c, CRC(c5ce4a41) SHA1(abbecddba4dd4691240df112138fa95f1e125a85) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890477.bin", 0x0000, 0x5000, CRC(a7dd60c5) SHA1(dfa3a89939cec2225d975984caf4703d83708b85) ) \
	ROM_LOAD( "95890478.bin", 0x0000, 0x5000, CRC(d24b24f1) SHA1(0c3a9c9579bb1066ea3e82e58b65d3bcac7fe900) ) \
	ROM_LOAD( "95890479.bin", 0x0000, 0x5000, CRC(4cf1e8ad) SHA1(924b454aa9c7340374b98201fbbaf922b836d779) )
#define sc_fwp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "5wayssnd.bin", 0x0000, 0x0f0833, CRC(0f165ccb) SHA1(709c91845cbe6b17c888e143eb121d649a221418) )

#define sc_grq_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008357.bin", 0x0000, 0x100000, CRC(c5d8c1ba) SHA1(b017c0b616b5ad2e54f111571b16067269a0e69f) )
#define sc_gldfv_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008347.bin", 0x0000, 0x100000, CRC(1d24f3ed) SHA1(01c7a8d2c8811cccee3748305681df75361f2254) )
#define sc_gldgo_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009166.bin", 0x000000, 0x100000, CRC(06ae879f) SHA1(b4357306f4361bd26532e3db399f105f09fdf5be) ) \
	ROM_LOAD( "95009167.bin", 0x100000, 0x100000, CRC(5e34266d) SHA1(35fa153ccb7fd9001cd23dc0c9e683291615e9e0) )
#define sc_gldsp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008543.bin", 0x0000, 0x0d877c, CRC(eabb0fd5) SHA1(260807cf2ce23e02bfaa7d880823e606f8240bee) )
#define sc_gmclb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009112.bin", 0x000000, 0x100000, CRC(80be159b) SHA1(2e6d19f97613152fcb17d027efc886815102fd56) ) \
	ROM_LOAD( "95009113.bin", 0x100000, 0x100000, CRC(edd3d30e) SHA1(94a56fda349d998b8571c271041d11cc79afaec2) )
#define sc_gamcs_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004285.snd", 0x0000, 0x080000, CRC(034a84a6) SHA1(f5b815d943fe9f7eaff4358657426be5260753bf) )
#define sc_game_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004285.snd", 0x0000, 0x080000, CRC(034a84a6) SHA1(f5b815d943fe9f7eaff4358657426be5260753bf) )
#define sc_ziggy_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008472.bin", 0x0000, 0x0ece7a, CRC(12a9ec95) SHA1(1c3634b27718b1ce496c02c7dd6ef29800d1d2ee) )
#define sc_gldcl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008968.bin", 0x0000, 0x0f4e4e, CRC(23e718a6) SHA1(1c781380d6b4fb307af6cb81be830159ce610352) )
#define sc_ghost_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008594.bin", 0x0000, 0x0b2ad7, CRC(3f7767d0) SHA1(b7ca39a42b59d1ac2088412fbb5f987b68f0189a) )
#define sc_ggrid_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008346.bin", 0x0000, 0x0e486b, CRC(1ec79154) SHA1(d6aff554e882e6f689caed383db79c752c9e672b) )
#define sc4ggcl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008400.bin", 0x0000, 0x0e486b, CRC(78704d20) SHA1(c5f62434785b10d2bff5bcf415c2236b4f1ee551) )
#define sc4goldo_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004165.bin", 0x000000, 0x080000, CRC(c52f4218) SHA1(2648f3ef35d68bdd14a0a773151157dae0271067) ) \
	ROM_LOAD( "95004166.bin", 0x080000, 0x080000, CRC(a77f1337) SHA1(e292c87fa8e643846cc0ddd7b9a585429b7efdd6) )


#define sc_gshot_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008904.bin", 0x0000, 0x0b32a1, CRC(6e11054e) SHA1(45ff06ce84cf1675747976b11e144a43d96977ac) )
#define sc_goldw_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008356.bin", 0x0000, 0x080000, CRC(c8f1ca20) SHA1(13136df35c09fea102c5176e249436f9e64616a5) ) /* golden winner */
#define PR1436_TRIPLE_CASINO_SOUNDS11 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008423.bin", 0x0000, 0x100000, CRC(4cab20ee) SHA1(e27221a94e54db340eaf7fc30e722b354c85686d) ) /* triple casino */

#define sc_gx_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004185", 0x0000, 0x080000, CRC(86756ea2) SHA1(86180e038a9bb3a3a700af1ddc571794e6948165) )
#define sc_gxcas_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004185", 0x0000, 0x080000, CRC(86756ea2) SHA1(86180e038a9bb3a3a700af1ddc571794e6948165) )

#define sc_gd_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* PR1016 GOLD DIG SOUNDS11 (game header actually lists PR1016 GOLD DIGGER SOUNDS11 but it accepts these) */ \
	ROM_LOAD( "95008187.bin", 0x000000, 0x100000, CRC(1fbbc7cc) SHA1(6e19b582a3654bbbcf65b8f42bd556029f43e8cb) ) \
	ROM_LOAD( "95008188.bin", 0x100000, 0x100000, CRC(a74b23a7) SHA1(f7948edfb5524eaf80039c07ca7a9f76883f5a6f) )
#define PR3509_GOLD_DIGGER_SOUNDS11 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* PR3509 GOLD DIGGER SOUNDS11 */ \
	ROM_LOAD( "95009061.bin", 0x000000, 0x100000, CRC(c4cad720) SHA1(ce67af8d9e0b2f4f79d38b7a01dfc5ff7323e162) ) \
	ROM_LOAD( "95009062.bin", 0x100000, 0x100000, CRC(509761d3) SHA1(6a133cc33bac6a1696de98a4961572a86cefc1c8) )

#define sc_gdmz_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008980.bin", 0x0000, 0x0fe431, CRC(c09277a0) SHA1(227609194f7d7e8b00cbd5cb94f8679f8fff71c6) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890471.bin", 0x0000, 0x5000, CRC(98fce599) SHA1(559534c3b33c6390a06b2e97e4c595055c8c4fea) ) \
	ROM_LOAD( "95890472.bin", 0x0000, 0x5000, CRC(ed6aa1ad) SHA1(83b69606398a30cb9c2ebc2f7406ef3215afab9e) ) \
	ROM_LOAD( "95890473.bin", 0x0000, 0x5000, CRC(73d06df1) SHA1(6959ee1b1c05398bc95430fea3590e5a86dd4312) )
#define sc_gdclb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008248.bin", 0x0000, 0x100000, CRC(abfba7ee) SHA1(95b61b34080196d745cc00ae32a333edb111da89) )

#define sc_gunp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008744.bin", 0x000000, 0x100000, CRC(be8313a2) SHA1(247ec0b2140d261bf5e7ab21cedb4ad2fd708ca7) ) \
	ROM_LOAD( "95008745.bin", 0x100000, 0x100000, CRC(355caf88) SHA1(82951c00efd27533ccbc78b46cd5b69be5c3e26d) )
#define sc_gag_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004132.1_1", 0x000000, 0x080000, CRC(17320a32) SHA1(113ab1db5227fe17053969dd98f94f0b3b7555f5) ) \
	ROM_LOAD( "95004132.1_2", 0x080000, 0x080000, CRC(03593875) SHA1(a5f6730664b1bbfdb7b98b41379e9b240f43c53c) )
#define sc_goud_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004288.sn1", 0x000000, 0x080000, CRC(ae6d0680) SHA1(467d0127e2a63ca031ebf2816dde99273b513913) ) \
	ROM_LOAD( "95004289.sn2", 0x080000, 0x080000, CRC(9c88f3c8) SHA1(1152077f59cc28726f1428a9fdaf2c92ddef6c73) )
#define sc_hirol_others \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890458.bin", 0x0000, 0x5000, CRC(237fc19f) SHA1(41c0dad102e8345c568f6178b13a17acedcbe1b3) ) \
	ROM_LOAD( "95890459.bin", 0x0000, 0x5000, CRC(56e985ab) SHA1(48c425f85a8feba6775f2b34be8c91c78c233d66) ) \
	ROM_LOAD( "95890460.bin", 0x0000, 0x5000, CRC(c85349f7) SHA1(7be87d120dc5d5972663ede3df6c9221deddff9b) ) \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008977.bin", 0x0000, 0x0d9fcb, CRC(99173f54) SHA1(86d6432a37453865f6ee3b80bf8e1c1c84c6e3f9) )
#define sc_hulk_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008367.bin", 0x000000, 0x100000, CRC(37eea219) SHA1(e612b2c0bca83d742b35fbaf487b6805cfac024d) ) \
	ROM_LOAD( "95008368.bin", 0x100000, 0x100000, CRC(06549054) SHA1(923a320f9689361802dc82db467b294f7bc85147) )
#define PR1327_CAS_HAPPY_NOTES_SOUNDS11 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008312.bin", 0x000000, 0x100000, CRC(cf33e14d) SHA1(33382be257a145784d9de7050b5c90f725241de4) )
#define sc_hapnt_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* PR1306 HAPPY NOTES SOUNDS11 */ \
	ROM_LOAD( "95008290.bin", 0x000000, 0x100000, CRC(427fc43d) SHA1(c63154c7f2bb3e7ec78e31268a8040d0eb131f2f) )
#define sc_hfcl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004272.bin", 0x000000, 0x080000, CRC(843cea31) SHA1(1d3a524600c8a0b7d6e3a32cad71f7f53c8190a1) ) \
	ROM_LOAD( "95004273.bin", 0x080000, 0x080000, CRC(c0782f35) SHA1(d763f7f8377c97129485522b201413e35c9c5760) )
#define sc_hill_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008494.bin", 0x0000, 0x100000, CRC(b2a2d2ca) SHA1(f9cd50c043dd58e4c92f94e4255c2fcb47963d39) )
#define sc_himi_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008268.bin", 0x0000, 0x0ff264, CRC(6d5e7041) SHA1(bcc7b5c000aa8c25818aa063cd74f998484224a2) )

#define sc_hiss_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008910.bin", 0x0000, 0x0fff2c, CRC(e5079d76) SHA1(0c44060d695e44720e7c34a9a142bdccd86ee943) )

#define sc_hotrd_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008571.bin", 0x0000, 0x100000, CRC(50331772) SHA1(84aa448320210bfa8ea656f59cc4a320a0a46a32) )
#define sc_hotsh_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR3211 HOT SHOT SOUNDS11 */ \
	ROM_LOAD( "95008794.bin", 0x000000, 0x100000, CRC(2bbe0adc) SHA1(1d5fb724ced3dbe8d744c2befae33fff69fbecba) ) \
	ROM_LOAD( "95008795.bin", 0x100000, 0x100000, CRC(0affc9f9) SHA1(0eea59a9e8293fb68172a9389d23488be508dda5) )

#define sc_hotwd_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* PR1311 HOT WAD SOUNDS11 */ \
	ROM_LOAD( "95008309.bin", 0x000000, 0x100000, CRC(8cd75778) SHA1(5d75c10b32477faa24c73ad03de75914385787f6) )
#define sc_celeb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008440.bin", 0x000000, 0x100000, CRC(72a98e5d) SHA1(5f3e441d6c3fa2373284cee704630028cf7800ad) ) \
	ROM_LOAD( "95008441.bin", 0x100000, 0x100000, CRC(d7382975) SHA1(f5a4d92f243ec745f91d008d623b43dfa024635c) )
#define sc_iab_others \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890446.bin", 0x0000, 0x5000, CRC(8fbf0046) SHA1(5e121f73fcf1777041995c9891465e17c37c8163) ) \
	ROM_LOAD( "95890447.bin", 0x0000, 0x5000, CRC(fa294472) SHA1(71fa4cfaf8ba81f446d9f3504123e3c0c9a9af90) ) \
	ROM_LOAD( "95890448.bin", 0x0000, 0x5000, CRC(6493882e) SHA1(c69401b35ef710d0df51541e615818dbd259c7f7) ) \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008969.bin", 0x0000, 0x0fe101, CRC(798c9559) SHA1(4afb21a78be73353741b5903099d3c48e44fcebb) )
#define sc_ijbdo_others \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95891076.bin", 0x0000, 0x5000, CRC(d535ed90) SHA1(0e349023bec422b356d7b9cf7f892a10fabbdedd) ) \
	ROM_LOAD( "95891077.bin", 0x0000, 0x5000, CRC(a0a3a9a4) SHA1(e74ec6d2729bbef737300a014e83409ef034f605) ) \
	ROM_LOAD( "95891078.bin", 0x0000, 0x5000, CRC(3e1965f8) SHA1(b37c73edf7f4b17fe3554fa394846929ad2031c0) ) \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008896.bin", 0x0000, 0x0eeaef, CRC(d581cb00) SHA1(16b44ba6cf4008109792a016d7aef12e6d6329d7) )
#define sc_inspn_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008909.bin", 0x0000, 0x0face9, CRC(e79bdfa0) SHA1(b31362a2d14c17b1367b45ea8734e408a2899fc2) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890327.bin", 0x0000, 0x5000, CRC(f63a2e4f) SHA1(2982130035f122a3d9a4f4a277bc272c9613562b) ) \
	ROM_LOAD( "95890328.bin", 0x0000, 0x5000, CRC(83ac6a7b) SHA1(e4aae883b1070278314c6d388a132d362a059c5b) ) \
	ROM_LOAD( "95890329.bin", 0x0000, 0x5000, CRC(1d16a627) SHA1(985d526178de176869fdad3c4264f47de14d4d7f) )
#define sc_ijob_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008764.bin", 0x000000, 0x100000, CRC(995c0c63) SHA1(7459e92ad80d4eb61008a011bf5ae3a1d5cb8f9d) ) \
	/* ROM_LOAD( "95008765.bin", 0x100000, 0x0fb251, CRC(3c2feab2) SHA1(797ef43d2166c0522f6da5dd17bf4443f08891a5) ) */ \
	ROM_LOAD( "95008765.bin", 0x100000, 0x100000, CRC(72702540) SHA1(72525c65d4d49ed311e14ba5fc476296e5dbfc8a) )
#define sc_jjok_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008470.bin", 0x000000, 0x100000, CRC(b5162561) SHA1(28feebc4de68264b92325e58bbf95458533bde7f) )
#define sc_kingx_others \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890374.bin", 0x0000, 0x5000, CRC(87f2ca06) SHA1(480be8b0a42c9be55cb9e4c264438667734eb644) ) \
	ROM_LOAD( "95890375.bin", 0x0000, 0x5000, CRC(9b243aba) SHA1(537783dff8f155921a378224f2a8fe754c4947a5) ) \
	ROM_LOAD( "95890376.bin", 0x0000, 0x5000, CRC(587e182a) SHA1(23de9e8b4d2631e53917346d4902813d474e64a0) ) \
	ROM_LOAD( "95890700.bin", 0x0000, 0x5000, CRC(fab9a6f1) SHA1(a7901e76e96f083ca65ffbd847b36e1140c6caaf) ) \
	ROM_LOAD( "95890701.bin", 0x0000, 0x5000, CRC(e66f564d) SHA1(7267fd9d4ad1b1886870eb89c940f59a8f84c014) ) \
	ROM_LOAD( "95890702.bin", 0x0000, 0x5000, CRC(253574dd) SHA1(15b9aac5e9a176f792eefcbd7de2ad8df38ff67b) ) \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008995.bin", 0x0000, 0x0931b3, CRC(e874bd15) SHA1(8adb32532dd053ca2c4e0987352013310ce197a0) ) /* = 95008995.bin          sc4ckx     Casino King X (Mazooma) (Scorpion 4) */
#define sc_ldvl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008866.bin", 0x0000, 0x0ff4f6, CRC(de0f7488) SHA1(dc9d546efa940f0cc69afb296bda0f07f82a4258) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890764.bin", 0x0000, 0x5000, CRC(15baa871) SHA1(9d3f021cfde738bc68d99f072160aee3eeaf8798) ) \
	ROM_LOAD( "95890765.bin", 0x0000, 0x5000, CRC(602cec45) SHA1(94f97824a90c4771e8c1fbab2cb41723b786f7be) ) \
	ROM_LOAD( "95890766.bin", 0x0000, 0x5000, CRC(fe962019) SHA1(e7283db78ac104bb47a37e5e81d50cd81db477f5) )
#define sc_lotrf_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008251.bin", 0x000000, 0x100000, CRC(2af25e80) SHA1(fb606e434ac99eee6e114d6639f28f0fa1e7ffb3) ) \
	ROM_LOAD( "95008252.bin", 0x100000, 0x100000, CRC(fbca1cfb) SHA1(27c610af14b10d1d72774186fd3afc12d5db925e) )
#define sc_lotr2_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008320.bin", 0x000000, 0x100000, CRC(8ba801e3) SHA1(4d6a009c2f4f4478276f1227af6315b4be90fc87) ) \
	ROM_LOAD( "95008321.bin", 0x100000, 0x100000, CRC(42482ddc) SHA1(304961f267b85bc599dd64c97c14f34b2913dd59) )
#define sc_ltr2c_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008342.bin", 0x000000, 0x100000, CRC(44f48bf6) SHA1(2b36fb9ca6eb9cb35cf67e580d736c711d96ea25) ) \
	ROM_LOAD( "95008343.bin", 0x100000, 0x100000, CRC(f35a7927) SHA1(f2a93c8ff4699c7e6572a7a43b6b2bc50683594c) )
#define sc_lotrt_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008251.bin", 0x000000, 0x100000, CRC(2af25e80) SHA1(fb606e434ac99eee6e114d6639f28f0fa1e7ffb3) ) \
	ROM_LOAD( "95008252.bin", 0x100000, 0x100000, CRC(fbca1cfb) SHA1(27c610af14b10d1d72774186fd3afc12d5db925e) )
#define sc_lotrr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008413.bin", 0x000000, 0x100000, CRC(66d6fd58) SHA1(dff5cc3f09fb4082fde4d5d4a9ecfe00cbce7242) ) \
	ROM_LOAD( "95008414.bin", 0x100000, 0x100000, CRC(8ec846ac) SHA1(3f66cbe5bc7190406425488beccb3e1d15356b1a) )
#define sc_luck7_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "lucky7s.bin", 0x0000, 0x200000, CRC(93002e7d) SHA1(50edf9064fb3bc9fd13efc65be6b12a4329fdb41) )
#define sc_monky_others \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890706.bin", 0x0000, 0x5000, CRC(4b2c5ee0) SHA1(e5bdcc4f076cc5550e51f3f4d4de5d8c59a4f545) ) \
	ROM_LOAD( "95890707.bin", 0x0000, 0x5000, CRC(3eba1ad4) SHA1(c780a4813c6b507a7e21ef2d7a401f3d764fd0f7) ) \
	ROM_LOAD( "95890708.bin", 0x0000, 0x5000, CRC(a000d688) SHA1(bcc197af2596b9c81cc56926fefc5854d7a94a27) ) \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008998.bin", 0x0000, 0x0f0a3e, CRC(cc0dd8f2) SHA1(c99c54ce1289604f57ce2345ec5cb69be91b69d6) )
#define sc_mobob_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008842.bin", 0x000000, 0x100000, CRC(3205f4c9) SHA1(a8b92e1558911e02281b1c38c64c5e04836794b0) ) \
	ROM_LOAD( "95008843.bin", 0x100000, 0x100000, CRC(95057e16) SHA1(6c26451dd336f908ce0fcea0c56176b589111868) )
#define sc_momil_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009110.bin", 0x000000, 0x100000, CRC(8ec67890) SHA1(ba7f8cd4315a4ca8d2adeedc40c50e9e9a9b38f6) ) \
	ROM_LOAD( "95009111.bin", 0x100000, 0x100000, CRC(de68000c) SHA1(be69779ccb213a95698c2eafafe1f4a339febc4f) )
#define sc_moms_others \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95891135.bin", 0x0000, 0x5000, CRC(3fdecc78) SHA1(271699c347bcd2b40282d22f9e1a4e36713845e1) ) \
	ROM_LOAD( "95891136.bin", 0x0000, 0x5000, CRC(4a48884c) SHA1(d641295cdcc0226c64797d24799d8508ef13f204) ) \
	ROM_LOAD( "95891137.bin", 0x0000, 0x5000, CRC(d4f24410) SHA1(a35803e69aa8a45d46409baebbcbc20a35c1ac5d) ) \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009023.bin", 0x0000, 0x0ffbf6, CRC(a10e8614) SHA1(85fc093f176cb81660717eb84301e9204402e83e) )

#define sc_mor2r_others \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95891090.bin", 0x0000, 0x5000, CRC(a5009c47) SHA1(38b08c39ded1f1baef8a01373f19694b49d032b7) ) \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009004.bin", 0x0000, 0x0ffe87, CRC(2403a082) SHA1(cd69739b2249724e3a88ea1c0b61151aeae0ea01) )
#define sc_monwa_others \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95891094.bin", 0x0000, 0x5000, CRC(a241259b) SHA1(94dfcc7371593d983c8b62dba085b3ec51a33ed1) ) \
	ROM_LOAD( "95891095.bin", 0x0000, 0x5000, CRC(d7d761af) SHA1(a7adda6fe7bcec0f40587273a9a1d16a220f400a) ) \
	ROM_LOAD( "95891096.bin", 0x0000, 0x5000, CRC(496dadf3) SHA1(997f91c4c96b8971cc3ccfcaaaa42af6aac42ddd) ) \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008010.bin", 0x0000, 0x0d5d57, CRC(1a75f403) SHA1(3bb5ca732d27aa4b8a2c96945be7e32c2abfe1c4) )
#define sc_cmcob_others \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95891127.bin", 0x0000, 0x5000, CRC(7e7d858c) SHA1(8fe2b07275ad36ae80149e091582f729c8a1d240) ) \
	ROM_LOAD( "95891128.bin", 0x0000, 0x5000, CRC(acfcec9c) SHA1(270b9ae893016d6272f599fa29402d911650b85f) ) \
	ROM_LOAD( "95891129.bin", 0x0000, 0x5000, CRC(c7299792) SHA1(f6279d507091d257a74b2419ec883c363b3c548f) ) \
	ROM_LOAD( "95891130.bin", 0x0000, 0x5000, CRC(15a8fe82) SHA1(7fb448b5559d8248a302b7b0cfaa7dfa07dc16dd) ) \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009021.bin", 0x0000, 0x0ff73b, CRC(068c715c) SHA1(25f80da3b548b9b0b6331d402d732a312985993b) )
#define sc_mogta_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009039.bin", 0x000000, 0x100000, CRC(0756b1b7) SHA1(82a8f9bf4c70a492ae2d4f478ecc73cccf9aecdd) ) \
	ROM_LOAD( "95009040.bin", 0x100000, 0x100000, CRC(9a928ef4) SHA1(5b19f70d067540aef3dc140115c12601d2b8176e) )
#define sc_monsp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* 95004291 MONEYSPINNER PR1158 */ \
	ROM_LOAD( "95004291.snd", 0x000000, 0x080000, CRC(0ea304f7) SHA1(3d524e108cd12e83956fec3997316a92a9e0898d) )


#define sc_monop_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* MPDX SOUNDS */ \
	ROM_LOAD( "95008514.bin", 0x0000, 0x0ed840, CRC(2608cbde) SHA1(4831570fbc825592781e6473173d6ec911a0054d) )
#define sc_mmm_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008944.bin", 0x0000, 0x0c76af, CRC(d2683fa2) SHA1(ebccd3da695cc69826493ac9e745b8dc2ce2ab8c) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890425.bin", 0x0000, 0x5000, CRC(fde2f6d4) SHA1(e62c28175138ed7d92c1e00c0a07a8e1f4a41ad2) ) \
	ROM_LOAD( "95890426.bin", 0x0000, 0x5000, CRC(8874b2e0) SHA1(0ba474cd47a65a5120df20b0f1ffcfe0aa51b0c5) ) \
	ROM_LOAD( "95890427.bin", 0x0000, 0x5000, CRC(16ce7ebc) SHA1(8561cf7808f09a92af8715999000684c1bef11bb) )
#define sc_mmad_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008519.bin", 0x0000, 0x0fe4d6, CRC(e16efe22) SHA1(144e93e01967f343607ee8cee6add3d8ac8f5f28) )
#define sc_mdm_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008844.bin", 0x000000, 0x100000, CRC(807916aa) SHA1(d266cdb2c4961337b1b1f5b6325a0b0bec934bad) ) \
	ROM_LOAD( "95008845.bin", 0x100000, 0x100000, CRC(e1e74669) SHA1(32cb963c3698a0659c23c0662da04a97539a3c28) )
#define sc_mhn_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008857.bin", 0x0000, 0x0f2fe8, CRC(a7f64802) SHA1(8a667ebf29feaffb2676ebd770b570c1015f3c2a) ) /* r2r sounds */
#define sc_mhp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008707.bin", 0x000000, 0x100000, CRC(88ffdb28) SHA1(724a2fe6703d4e5aab7d3989be9d3ae4dc8ab057) ) \
	ROM_LOAD( "95008708.bin", 0x100000, 0x100000, CRC(fa8aa99e) SHA1(bd4dd726b0af371d9b8589de461316990c3232dc) )
#define sc_mmb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR1911 MPOLY MONEYBAGS SOUNDS11 */ \
	ROM_LOAD( "95008612.bin", 0x000000, 0x100000, CRC(5f76152c) SHA1(c9f0ac000ec0b11ac3ba5eb5ff7e3dc2084f6744) ) \
	ROM_LOAD( "95008613.bin", 0x100000, 0x100000, CRC(df266121) SHA1(ece6b5a42ec3d18d97cffe3e6c98c6e6cd34682b) )
#define PR3413_MPOLY_MB_SOUNDS11 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR3413 MPOLY MB SOUNDS11 */ \
	ROM_LOAD( "95009168.bin", 0x000000, 0x100000, CRC(1ad311a5) SHA1(bd7728423cd855ea58fe21097d3d3db5a7e08339) ) \
	ROM_LOAD( "95009169.bin", 0x100000, 0x100000, CRC(23a8705f) SHA1(a36e71706cee64b56958fe4aefba8b85d2369e38) )
#define sc_mrh_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008852.bin", 0x0000, 0x0c845b, CRC(265db01b) SHA1(6b9b62eded887ec528bb53b255d539122124eccc) ) \
	\
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890721.bin", 0x0000, 0x5000, CRC(766aaa4d) SHA1(bd1bfd7209e4d54e1fc798b720b749dd7ce8f9e6) ) \
	ROM_LOAD( "95890722.bin", 0x0000, 0x5000, CRC(6abc5af1) SHA1(61ef7254ab948899df9de693227e0eff2ad22e74) ) \
	ROM_LOAD( "95890723.bin", 0x0000, 0x5000, CRC(a9e67861) SHA1(2ce81fb21e709438c63d5eb74d107e6dc1905dd2) )
#define sc_mr2r_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008893.bin", 0x0000, 0x0f2fe8, CRC(a7f64802) SHA1(8a667ebf29feaffb2676ebd770b570c1015f3c2a) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890480.bin", 0x0000, 0x5000, CRC(3618777f) SHA1(97ab5be3bcdbe1c94e638c4dceac69bbfe359a0e) ) \
	ROM_LOAD( "95890481.bin", 0x0000, 0x5000, CRC(438e334b) SHA1(69b132ea3cbe31d31fd6fa6b915ddc2f4e66f068) ) \
	ROM_LOAD( "95890482.bin", 0x0000, 0x5000, CRC(dd34ff17) SHA1(b057c4a70066e8e8f9657e16adc4e0bfb6f00494) )
#define sc_mcas_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* MONO SOUNDS */ \
	ROM_LOAD( "95008991.bin", 0x0000, 0x0ec7b3, CRC(fad36ca8) SHA1(67918353945429183befba56ab891a1d138f2e8b) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890727.bin", 0x0000, 0x5000, CRC(10ec3f00) SHA1(986bfce992e113bab26eea8d1981905a54367b94) ) \
	ROM_LOAD( "95890728.bin", 0x0000, 0x5000, CRC(0c3acfbc) SHA1(ff39c8236d5bd44c7d238edd1827af5aebc46e5c) ) \
	ROM_LOAD( "95890729.bin", 0x0000, 0x5000, CRC(cf60ed2c) SHA1(df015b71567d6762b4c8ea6ad6d719cefdfdc60f) )
#define sc_mono_alt_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* MONO SOUNDS */ \
	ROM_LOAD( "95008436.bin", 0x0000, 0x0e6369, CRC(32e8df77) SHA1(fa044287970d4e54af53ff492eabc50afa9a7a92) )

#define PR1706_WHEEL_OF_WEALTH_SOUNDS11 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008550.bin", 0x000000, 0x100000, CRC(db6343bf) SHA1(e4d702020af67aa5be0560027706c1cbf34296fa) )
#define WHEEL_OF_WEALTH_WOWT_SOUNDS \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008869.bin", 0x000000, 0x0b9d9d, CRC(f3ef3bbb) SHA1(92f9835e96c4fc444a451e97b2b8a7b66e5794b7) )

#define sc_mowow_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* WOW SOUNDS - wrong for this game, wants "WOTW SOUNDS" */ \
	ROM_LOAD( "95008850.bin", 0x000000, 0x0af41f, CRC(8ca16e09) SHA1(9b494ad6946c2c7bbfad6591e62fa699fd53b6dc) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890712.bin", 0x0000, 0x5000, CRC(ec6db00b) SHA1(d16a1527caa3c115e3326c897ce0fa66e3a0420d) ) \
	ROM_LOAD( "95890713.bin", 0x0000, 0x5000, CRC(f0bb40b7) SHA1(33c19dab3086cdeae4f503fbf3f3cc5f0dad98c4) ) \
	ROM_LOAD( "95890714.bin", 0x0000, 0x5000, CRC(33e16227) SHA1(87efc1a046ef6af0b72cc76a6ee393a4d1ddbce3) )

/* the game accepts either of these pairs, what's the difference? */
#define sc_nmare_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* PR3002 ELM STREET SOUNDS11 (alt checksum..) */ \
	ROM_LOAD( "95008703.bin", 0x000000, 0x100000, CRC(5c095089) SHA1(ffba71efccf4679c2b29ab620e67f3fa45ff19d4) ) \
	ROM_LOAD( "95008704.bin", 0x100000, 0x100000, CRC(61e5d6f4) SHA1(6d221e0b3dce7507253bb7f07d99d03025d813cb) ) \
	ROM_REGION( 0x400000, "altymz", ROMREGION_ERASE00 ) \
	/* PR3002 ELM STREET SOUNDS11 */ \
	ROM_LOAD( "95008664.bin", 0x000000, 0x100000, CRC(54d245d7) SHA1(503db10681bc1d6cf1cffb2a6f818c5d912a7f56) ) \
	ROM_LOAD( "95008665.bin", 0x100000, 0x100000, CRC(79a00527) SHA1(b8207d3a81e97b2233fea1df883a4128388570e4) )



#define sc_nunsm_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008522.bin", 0x0000, 0x0f9907, CRC(df612d06) SHA1(cbca56230c4ad4c6411aa5c2e2ca2ae8152b5297) )
#define sc_rainb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009122.bin", 0x000000, 0x100000, CRC(7197027f) SHA1(216e43135c00452556ad5e7647891f14641840ed) ) \
	ROM_LOAD( "95009123.bin", 0x100000, 0x100000, CRC(3454f17d) SHA1(fc45d2653a0f260702a94663939b0d0176fc1c87) )
#define sc_parot_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008736.bin", 0x000000, 0x100000, CRC(148a7600) SHA1(d57f4a667151e78715d617b3d1e963d29d83b538) ) \
	ROM_LOAD( "95008737.bin", 0x100000, 0x100000, CRC(4e61167f) SHA1(6faa7a395bd2d9067b88a9c9c365198ea26ebb39) )
#define sc_pilep_others \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95891066.bin", 0x0000, 0x5000, CRC(c058097e) SHA1(f6940ac1fe02498d13d62bd4318fd7445880c9a9) ) \
	ROM_LOAD( "95891067.bin", 0x0000, 0x5000, CRC(b5ce4d4a) SHA1(918009698696ce6bd9d0d68b4dc3e4234ff552bb) ) \
	ROM_LOAD( "95891068.bin", 0x0000, 0x5000, CRC(2b748116) SHA1(f6100c3ce79b4456d2f7fcda8e48734aad48141c) ) \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008892.bin", 0x0000, 0x0fe398, CRC(e59abfb2) SHA1(fbe9ec614774a46f93ffb3ef9b8d4ee430d0aeed) )
#define sc_potsm_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008966.bin", 0x0000, 0x0ff8c9, CRC(1cc159e9) SHA1(f7f5a027806c561825656c5741166e0893a30b10) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890443.bin", 0x0000, 0x5000, CRC(d6ee4a71) SHA1(f638c1e150ae089533ea43ca66010cb7190eb646) ) \
	ROM_LOAD( "95890444.bin", 0x0000, 0x5000, CRC(a3780e45) SHA1(4babae846051a0199f0e0baa6de0c7b16c73a185) ) \
	ROM_LOAD( "95890445.bin", 0x0000, 0x5000, CRC(3dc2c219) SHA1(acad92fd6d5112a5366b609761fbd2907bbce91a) )
#define sc_cpen1_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009057.bin", 0x000000, 0x100000, CRC(7b83611b) SHA1(6163ab8e848f019190af446710b31e3516386c57) ) \
	ROM_LOAD( "95009058.bin", 0x100000, 0x100000, CRC(43858a26) SHA1(fdb831e6a2ba161e72883100c22e2e9eee335fe6) )
#define sc_potp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008483.bin", 0x0000, 0x100000, CRC(d208755d) SHA1(f87b9b7dc077f4a1bf141ae8ca1e985796c4bada) )

// these 3 are for the adder 5 game
#define SD_PANTHER_SNDS \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* SD_PANTHER SNDS */ \
	ROM_LOAD( "95008948.bin", 0x0000, 0x08b98b, CRC(35e995c5) SHA1(9d12a3002423e9434bcd76fe13f9fc0891354e40) )
#define PINK_PANTHER_SOUNDS \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PINK PANTHER SOUNDS */ \
	ROM_LOAD( "95008942.bin", 0x0000, 0x0ebc96, CRC(42765fbc) SHA1(ab5b9240a8e71695897fd55a1385b64f63ce52c8) )
#define PINKP_SOUNDS \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PINKP_SOUNDS */ \
	ROM_LOAD( "95008953.bin", 0x0000, 0x0fabc8, CRC(12d96e7a) SHA1(4f5179b5817e471b891f377f0bec81dcb4a2077c) )


#define sc_ppcr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008950.bin", 0x0000, 0x0fabc8, CRC(291b1b0d) SHA1(9d4149d2eb1303742dcba6e0e6d1b914f8a4019e) )
#define sc_ppctc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008785.bin", 0x000000, 0x100000, CRC(4f2284e6) SHA1(4a85bce29d9ae7181aef8874d46928e397ba915a) ) \
	ROM_LOAD( "95008786.bin", 0x100000, 0x100000, CRC(2c96e78e) SHA1(feab56483b6075ffcf8f60f8c573198ee91e7d64) )
#define sc_ppdym_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* DYMN SOUNDS */ \
	ROM_LOAD( "95008984.bin", 0x0000, 0x0e6770, CRC(8257c559) SHA1(9088fd4d28ad9701767a2a255164e7201410c33a) ) \
	/* DYMT SOUNDS */ \
	ROM_LOAD( "95008989.bin", 0x0000, 0x0e6770, CRC(fb60e407) SHA1(50f7ed9d2106db2f1e746081d7ca768177e7680c) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890483.bin", 0x0000, 0x5000, CRC(0ee68077) SHA1(163a46b16da238123cd542a62119dbaa726fca1b) ) \
	ROM_LOAD( "95890484.bin", 0x0000, 0x5000, CRC(123070cb) SHA1(4f3b313c3f7bd43120dd390fd9a187523838f559) ) \
	ROM_LOAD( "95890485.bin", 0x0000, 0x5000, CRC(d16a525b) SHA1(7358a180b1db0e2c5f4442481e9483a1170974a1) )
#define sc_ppsag_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008975.bin", 0x0000, 0x100000, CRC(0203bef7) SHA1(01b1f3225eb97303a3ddc678ad51aef90d2bb84a) )
#define sc_pony_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008408.bin", 0x0000, 0x100000, CRC(a28ef50e) SHA1(c5080418585a37ea63372b427c7e67b8c010f40c) )
#define sc_popey_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008798.bin", 0x000000, 0x100000, CRC(916cb144) SHA1(c75fa7a27be4e8dbd85fa074f6228877b5c86fc9) ) \
	ROM_LOAD( "95008799.bin", 0x100000, 0x100000, CRC(2f4c8eb6) SHA1(ca02b32705352735c4217aa20e9bc82eac8c3645) )

#define sc_pwrbl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008496.bin", 0x0000, 0x100000, CRC(66b32a17) SHA1(3adaa7067f91c071ca70e4c8b1c3d07f0402e1bb) )
#define sc_qual_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008354.bin", 0x0000, 0x100000, CRC(1fb18180) SHA1(c21456253b670ce140efeaff6a590f8a50112bd4) )
#define sc_quidv_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008993.bin", 0x0000, 0x0fab1c, CRC(e99defa5) SHA1(43954ec9acfedc86794d577d1e3058320264471e) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890703.bin", 0x0000, 0x5000, CRC(c87e0232) SHA1(c8de2c6b4f327ffd5b6bdb1248df1ba5573b21f0) ) \
	ROM_LOAD( "95890704.bin", 0x0000, 0x5000, CRC(bde84606) SHA1(0fe8285d862b97fee2a5a43082294a061b01bb16) ) \
	ROM_LOAD( "95890705.bin", 0x0000, 0x5000, CRC(23528a5a) SHA1(ac5b5a16a1e96a600be3760bd82cae2903459c24) )

#define sc_rhog_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008783.bin", 0x000000, 0x100000, CRC(50d2c89b) SHA1(f3e592f009765118009f97bd7d0951b72ffc2898) ) \
	ROM_LOAD( "95008784.bin", 0x100000, 0x100000, CRC(88185c65) SHA1(b3a6d06aca91dc04d9efbc1e4ff89b37bcf87a57) )
#define sc_rhclb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008802.bin", 0x000000, 0x100000, CRC(36647f4a) SHA1(ffa0cdf4f2b8ccc9afda67160a50e603b2a41ae6) ) \
	ROM_LOAD( "95008803.bin", 0x100000, 0x100000, CRC(2d26ff8a) SHA1(b9c797dfb2b06b3558e9a02d197e046244f4f2fc) )
#define sc_rdrag_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008485.bin", 0x0000, 0x0e7e1d, CRC(037269cd) SHA1(7b528427740c9f1b28e00d9b45ebeb5e77ca9ead) ) /* red dragon */
#define sc_rdrcl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008525.bin", 0x0000, 0x0e7e1d, CRC(0c7aa11d) SHA1(3d028d2d5558056e84626ba66f7c22286be34bd9) ) /* club red dragon */


#define sc_rhx_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* REDX SOUNDS */ \
	ROM_LOAD( "95008331.bin", 0x000000, 0x100000, CRC(b41748dd) SHA1(458409a4d2f6444af564ebe28d80e110fdb845b6) )
#define RHRL_SOUNDS \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* RHRL SOUNDS */ \
	ROM_LOAD( "95008927.bin", 0x000000, 0x0f1c08, CRC(38fccc48) SHA1(9ef11b3fbe3bbd3c5b74cee2c35581f3e41cb372) )
#define sc_rhxcl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008313.bin", 0x0000, 0x0c8958, CRC(4850fb2b) SHA1(9dde245c5b4afd6dbfd33372dff35fcbeefc2fe4) )


#define sc_relcz_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008185.bin", 0x000000, 0x100000, CRC(d41a9b86) SHA1(9269000ec8d81b24e04a98b63e3294d56439820d) ) \
	ROM_LOAD( "95008186.bin", 0x100000, 0x100000, CRC(8f5bfdaf) SHA1(ecba8cacc83224d75b8076c6fd5fe46863a0689e) )
#define sc_rotc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "rotcsnd.bin", 0x0000, 0x100000, CRC(301147a2) SHA1(49a31e1e3c8af19e524f4f4b68533e4b2cc5e03d) )



#define sc_rttt_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008306.bin", 0x0000, 0x100000, CRC(9c61f2ff) SHA1(5f000dcb2ec694fa8578f32fa485e61ad065e3d5) )
/* used by Rocket Science and Blast Off */
#define sc_roksc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* ROCKETSCIENCESND */ \
	ROM_LOAD( "95008405.bin", 0x0000, 0x0fec18, CRC(847c5cbb) SHA1(cd304c31bd1f87e6b2444b50450ab9dd42ca79eb) )



#define sc_rhxcs_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008853.bin", 0x0000, 0x0d11f1, CRC(c055b73c) SHA1(0c668e1733a089f95c61d1fe50fde2d4217e0e5f) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890724.bin", 0x0000, 0x5000, CRC(118e80ca) SHA1(9cd3dc111710224a8916f7b0f24e8997a9d98a8c) ) \
	ROM_LOAD( "95890725.bin", 0x0000, 0x5000, CRC(0d587076) SHA1(b470b3f1c1c82526c9dd08ae7dedbdc38bfb334d) ) \
	ROM_LOAD( "95890726.bin", 0x0000, 0x5000, CRC(ce0252e6) SHA1(9d6f69de70647e12d8352bc73922393006602797) )
#define sc_redsq_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008920.bin", 0x0000, 0x0f8d12, CRC(ac69f283) SHA1(4b62d2759f82effea988310df5ca92dd9f417a6d) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890350.bin", 0x0000, 0x5000, CRC(3e015e16) SHA1(c3a180d3611f46c9d7e93309d751b2422ec672cc) ) \
	ROM_LOAD( "95890351.bin", 0x0000, 0x5000, CRC(4b971a22) SHA1(205feec301bb69bf461fc4377704f675b5664944) ) \
	ROM_LOAD( "95890352.bin", 0x0000, 0x5000, CRC(d52dd67e) SHA1(8e5623d1fe4e096b1a19a153b0d2184424642469) )
#define sc_rich_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008085.hi", 0x000000, 0x100000, CRC(eb748aa3) SHA1(b81acfbceb5ff9df8bb30d3da82deea70bfce2fe) ) \
	ROM_LOAD( "95008084.lo", 0x100000, 0x100000, CRC(2231c842) SHA1(2180109949281b053313c8a8de33496d31d4279e) )

#define sc_rosts_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008820.bin", 0x000000, 0x100000, CRC(31c0d215) SHA1(7d0172de75e54a7c00b7c28e21d738e64597f4f5) ) \
	ROM_LOAD( "95008821.bin", 0x100000, 0x100000, CRC(9c934f26) SHA1(2598870f724a2b070c5f791a2d30e582f87c06da) )
#define sc_rovrt_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008961.bin", 0x0000, 0x0feba0, CRC(174f22f8) SHA1(e96c7019a48f1499c4b766e1fb700b2d4db59d5e) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890431.bin", 0x0000, 0x5000, CRC(a3d4624e) SHA1(4d66727af8cd1f10708635655777d80fe99e02af) ) \
	ROM_LOAD( "95890432.bin", 0x0000, 0x5000, CRC(d642267a) SHA1(9f2d0cd87e6acfebd855a7669c862dc20f1a52e3) ) \
	ROM_LOAD( "95890433.bin", 0x0000, 0x5000, CRC(48f8ea26) SHA1(156a83c1980483b12577fdb680d85a3f0f7bfdbd) )
#define sc_sharp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008561.bin", 0x000000, 0x100000, CRC(98d21491) SHA1(e06156dbe83fc26968da1a2432d697d68e6b88ee) ) \
	ROM_LOAD( "95008562.bin", 0x100000, 0x100000, CRC(fb3a25e7) SHA1(e8a60e4e870438fd4a330453ce1bdf8ad836fbc0) )

#define sc_srrqp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95891091.bin", 0x0000, 0x5000, CRC(e8a89f86) SHA1(0260292e43320b4d7a7cd93c9c058be6250a1441) ) \
	ROM_LOAD( "95891092.bin", 0x0000, 0x5000, CRC(9d3edbb2) SHA1(6a1f3030dc6438da5e83f1d9144bd7f4fab98c97) ) \
	ROM_LOAD( "95891093.bin", 0x0000, 0x5000, CRC(038417ee) SHA1(5e4234bf9b7d698b0f82af702209789948456e32) )
#define sc_witwi_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008756.bin", 0x000000, 0x100000, CRC(bde21cff) SHA1(fc8945e66416447d690515161acf9260620d7768) ) \
	ROM_LOAD( "95008757.bin", 0x100000, 0x100000, CRC(7c33cf90) SHA1(f3ad90c5623d97e3bf013927945383115042b84b) )
#define sc_spnrn_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008538.bin", 0x0000, 0x0de67f, CRC(5f689b5c) SHA1(a0469987b93fdd0c661ef622403e03c6edbc561a) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890178.bin", 0x0000, 0x5000, CRC(bb0186be) SHA1(575f212b555035a6a30fea7f49836f4ad72868d2) ) \
	ROM_LOAD( "95890179.bin", 0x0000, 0x5000, CRC(ea390914) SHA1(ac34e41f6e277115e5fc2ea2bb1675c1d7df0dfe) ) \
	ROM_LOAD( "95890180.bin", 0x0000, 0x5000, CRC(7483c548) SHA1(06675caa6e0e4f136995d542210bddbb2e9e2ba4) )


#define sc_sleut_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008870.bin", 0x0000, 0x0fa9dd, CRC(9b219ee5) SHA1(a488919d305bc8a2f8d92ea73054ea9e467dbb4e) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890777.bin", 0x0000, 0x5000, CRC(12e0a0db) SHA1(f7360b7f7086111dd951c9742f4b0402dfdd988c) ) \
	ROM_LOAD( "95890778.bin", 0x0000, 0x5000, CRC(6776e4ef) SHA1(a39e7aefb57521e4782dd0a8b37c904b7b76ff29) ) \
	ROM_LOAD( "95890779.bin", 0x0000, 0x5000, CRC(f9cc28b3) SHA1(031df3658b5aec9f318f42f04e8041fd9ae1b761) )


#define sc_showt_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR3013 SHOWTIME SOUNDS11 */ \
	ROM_LOAD( "95008734.bin", 0x000000, 0x100000, CRC(8b469657) SHA1(514b3f4cbbafb3cee618b9ce0bae572f1a4fcf7b) ) \
	ROM_LOAD( "95008735.bin", 0x100000, 0x100000, CRC(fa614cc1) SHA1(e5ecce4936a8db267b08b938ab5bc7500cf98f99) ) \
	ROM_REGION( 0x400000, "others2", ROMREGION_ERASE00 ) /* PR3243 SHOWTIME SOUNDS11 */ \
	ROM_LOAD( "95008777.bin", 0x000000, 0x100000, CRC(1ff19229) SHA1(6db08982ee923ef4568caf76e05088574f99fc36) ) \
	ROM_LOAD( "95008778.bin", 0x100000, 0x100000, CRC(2f2f3ee1) SHA1(cade826fec6c07a1fffa548c96404da83db14dd7) )
#define sc_slad_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008375.bin", 0x0000, 0x100000, CRC(967e41ec) SHA1(4ca9eb1db89b6f918b416f1c68805c4306850c7a) )
#define sc_srrmz_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008155.bin", 0x000000, 0x100000, CRC(2c12d323) SHA1(78199130f15b90f2c8f932f40d7e1e0e0646ce57) ) \
	ROM_LOAD( "95008156.bin", 0x100000, 0x100000, CRC(775dd1cd) SHA1(2b0ffa326ae53b60dbda6eb1f066b2ea29e6dd26) )


#define sc_sahed_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008353.bin", 0x0000, 0x0fbc01, CRC(593626e0) SHA1(f348d6ea347efa706b1587235241231562c447d3) )
#define sc_sirpz_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "sound_1", 0x000000, 0x080000, CRC(e5e1b0d5) SHA1(bfdc90e09534b97d9a6b6804284c80800db1aff7) ) \
	ROM_LOAD( "sound_2", 0x080000, 0x080000, CRC(3f8c71ed) SHA1(3bab20e8f6af2cfb8c7bba04b29f5aec4c692c1b) )
#define sc_smk7_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004163.lo", 0x000000, 0x080000, CRC(fd8fd1a9) SHA1(a6c1bfefc7200f9ad429bf166d0deeb6092e9e4d) ) \
	ROM_LOAD( "95004164.hi", 0x080000, 0x080000, CRC(9d8d69d3) SHA1(345ea1b517828cd4b140ec91e7014176cd45f6ad) )
#define sc_smtm_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95416744.bin", 0x0000, 0x100000, CRC(3aa90fed) SHA1(d399f67bbe1d6f58d4c0a9e322156055cf3dccdc) )
#define sc_spice_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008585.bin", 0x0000, 0x100000, CRC(2f15c4a4) SHA1(df543cd6ebecbbd3922bb8550e4bf349fd8eb45c) )


#define sc_sus_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008960.bin", 0x0000, 0x0f4505, CRC(a3279a7d) SHA1(da3cc1f3c0b9301e9b4d9f5253cafa3a9385be56) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890413.bin", 0x0000, 0x5000, CRC(5cdcd7cc) SHA1(7eaf28a20ca68c7edda1ee0cb5efefd00831c024) ) \
	ROM_LOAD( "95890414.bin", 0x0000, 0x5000, CRC(294a93f8) SHA1(f81f4cdb1b7e9107835f54702772c4e5f18d9731) ) \
	ROM_LOAD( "95890415.bin", 0x0000, 0x5000, CRC(b7f05fa4) SHA1(4cfbb0c7752edc6a0df8f9b6ab77f3fb9cd8db8e) )


#define sc_srr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008213.bin", 0x0000, 0x100000, CRC(e8f82b1d) SHA1(9357f587d638289b2cd5029e5895f69097d69089) )
#define sc_sslam_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008731.bin", 0x0000, 0x100000, CRC(b6f92b76) SHA1(69be21d12940dc415816dd3c77f4eb2c1bd1a555) )
#define sc_swbak_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008929.bin", 0x0000, 0x0f973a, CRC(016f4cc0) SHA1(763c6bf389e0c1e83d903d7aec23ac1a53261e7c) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890368.bin", 0x0000, 0x5000, CRC(01900e28) SHA1(c3e86b917b48249bdf0f1072f0cf5fa4b749d834) ) \
	ROM_LOAD( "95890369.bin", 0x0000, 0x5000, CRC(74064a1c) SHA1(0c254916fd56026dfd0df2abb4dc1c0b2dff1a7d) ) \
	ROM_LOAD( "95890370.bin", 0x0000, 0x5000, CRC(eabc8640) SHA1(41a5f5530289672563210d3ef71eb896cf6b3d5d) )
#define sc_tempt_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* PR1963 TEMPTATION SOUNDS11 */ \
	ROM_LOAD( "95008654.bin", 0x000000, 0x100000, CRC(9963724c) SHA1(90d22b11934e95fd2bfda026dcde51dcd74f93cc) ) \
	ROM_LOAD( "95008655.bin", 0x100000, 0x100000, CRC(12735319) SHA1(166fbcc5828fd26ace63eed127427cbd0c81570d) )

#define sc_typ_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008662.bin", 0x000000, 0x100000, CRC(51959c94) SHA1(8a4b68460da4223071e33158747ae0ea18b83a52) ) \
	ROM_LOAD( "95008663.bin", 0x100000, 0x100000, CRC(839687b2) SHA1(a9fd3645eb903cb52b348dd8313f0d953d5a5ec0) )


#define sc_vamp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008890.bin", 0x0000, 0x0ffd25, CRC(bdf5491b) SHA1(231264d83a84fcc54bf45a43a37464fc5c604d9b) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95891062.bin", 0x0000, 0x5000, CRC(3816210d) SHA1(890eed46cf9d7285887d0fe422ff6c8d8bf35820) ) \
	ROM_LOAD( "95891063.bin", 0x0000, 0x5000, CRC(4d806539) SHA1(73958733c49fa020f6425aef511489965df13907) )


#define sc_viz_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008859.bin", 0x000000, 0x100000, CRC(86e4b4a1) SHA1(21344c9e5757d8d9ad7f1cd5ce5deb9f93d18fcc) ) \
	ROM_LOAD( "95008860.bin", 0x100000, 0x0ae3ea, CRC(0e49df2c) SHA1(629b90f91e8840560ea6eaf0e94d6b2fb6aba71c) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890742.bin", 0x0000, 0x5000, CRC(247f75ec) SHA1(92d63123aabf6932b283bbbd581c1bbe526318ce) ) \
	ROM_LOAD( "95890743.bin", 0x0000, 0x5000, CRC(51e931d8) SHA1(91ceb930eac97e090de5471c8a789ddd31c65a10) ) \
	ROM_LOAD( "95890744.bin", 0x0000, 0x5000, CRC(cf53fd84) SHA1(0de52e4ce83f41f2928866fc914ed36023415a4f) )
#define sc_wacky_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* PR3330 WACKY RACES SOUNDS11 */ \
	ROM_LOAD( "95008834.bin", 0x000000, 0x100000, CRC(def73602) SHA1(681aafa4c035fdbb0f03e92fa56d31c13e27ca59) ) \
	ROM_LOAD( "95008835.bin", 0x100000, 0x100000, CRC(9a10ff4f) SHA1(903e43d9db726dc5ee92abaf23b61454611064b1) ) \
	/* PR3330 WACKY RACES SOUNDS12 */ \
	ROM_LOAD( "95009128.bin", 0x000000, 0x100000, CRC(d9883f0c) SHA1(3cfeb470248a9d5d6b653175c16f88c54d51fceb) ) \
	ROM_LOAD( "95009129.bin", 0x100000, 0x100000, CRC(969f418a) SHA1(ca65dfa23f9e6a516bc65baa8ddd80faa530af1f) )
#define sc_wotw_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008925.bin", 0x0000, 0x0eea8c, CRC(9cb8a972) SHA1(9b99eea5264fa0764f4598330f442335a8024cdb) )
#define sc_wca_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008524.bin", 0x0000, 0x0ec9a6, CRC(a585c613) SHA1(dbe4f3b8584b92012e84d48c232b31a0fba6e20c) )
#define sc_wok_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008811.bin", 0x000000, 0x100000, CRC(0aae3aea) SHA1(a13ca57b4ff95022392b6bf06065593c3271637a) ) \
	ROM_LOAD( "95008812.bin", 0x100000, 0x100000, CRC(4a48eabd) SHA1(30dc0cccc636c1472b7577353c1db8e88242bba4) )
#define sc_pircl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009200.bin", 0x000000, 0x100000, CRC(45392f4b) SHA1(9a71834e52480f87ab303c22efe15157b8f25b5c) ) \
	ROM_LOAD( "95009201.bin", 0x100000, 0x100000, CRC(3aece97c) SHA1(fc469870fef892da0a4aa49077c7746315933d40) )

#define PR1034_GOLDEN_BALLS_SOUNDS11 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008161.bin", 0x000000, 0x080000, CRC(ecd13fd9) SHA1(51d11b9133d4e840ce9afd7cf716520ea0fc0343) ) \
	ROM_LOAD( "95008162.bin", 0x080000, 0x080000, CRC(b4b4a5c5) SHA1(d0748decfaee7da52d2f6a4bc0877be4243ed6fb) )
#define sc_gball_pthers \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* PR1604 GOLDEN BALLS SOUNDS12 */ \
	ROM_LOAD( "95008465.bin", 0x000000, 0x100000, CRC(5d1fa2c9) SHA1(c12de2b89f0bcb8f1b35630fffd205fd9d5b9777) ) \
	ROM_LOAD( "95008466.bin", 0x100000, 0x100000, CRC(418068ab) SHA1(342939e9bcc1d213bc2f52666cc3765442e18635) )
#define sc_gfev_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95416146.bin", 0x0000, 0x100000, CRC(17e92fa0) SHA1(7dea5166f3f70e5d249da56f01bbe2267ce43d6a) )
#define sc_ggame_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008992.bin", 0x0000, 0x0aa536, CRC(aad10089) SHA1(d8a32f66432ee901be05435e8930d3897f4b4e33) ) /* BARX */ \
	ROM_REGION( 0x400000, "others", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008515.bin", 0x0000, 0x0ab5c3, CRC(70cd8480) SHA1(2da34a0c1d9d84471005f5d6491499e707c0b0d4) ) /* KGXDX */  \
	ROM_LOAD( "95008958.bin", 0x0000, 0x0435d3, CRC(31ffdb64) SHA1(e48cfa2e5b158555b4ba204fc1175810b81cbbed) ) /* MULTIGG */ \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890410.bin", 0x0000, 0x5000, CRC(9588ae1d) SHA1(ae45d9e0272b2b048b99e337def1acfb2524597e) ) \
	ROM_LOAD( "95890411.bin", 0x0000, 0x5000, CRC(895e5ea1) SHA1(070df49baca709f69fa1e522b21e42b716af0ba5) ) \
	ROM_LOAD( "95890412.bin", 0x0000, 0x5000, CRC(4a047c31) SHA1(aeb969801c89b60a644b4ffc2e1bbb73f6d61643) )
#define sc_ggg_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009015.bin", 0x0000, 0x0b7821, CRC(8b6ca362) SHA1(4c50935b6d1038738ce631fbdc359416197b8c03) ) /* BARX SOUNDS */ \
	ROM_REGION( 0x400000, "others", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008992.bin", 0x0000, 0x0aa536, CRC(aad10089) SHA1(d8a32f66432ee901be05435e8930d3897f4b4e33) ) /* BARX SOUNDS */ \
	ROM_REGION( 0x400000, "pivs", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95890730.bin", 0x0000, 0x5000, CRC(9673d1ce) SHA1(fee90139fc8de5e7b6dfe741b4852a363c17eb93) ) \
	ROM_LOAD( "95890731.bin", 0x0000, 0x5000, CRC(8aa52172) SHA1(441a649e3da00556a8ea966a88ee9b58b4943d3b) ) \
	ROM_LOAD( "95890732.bin", 0x0000, 0x5000, CRC(49ff03e2) SHA1(f8b77c97f2b1bb5de06f4c9835275ae9b83d0988) )

#define SC4_95004211_HELLS_BELLS_PR6945 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004211.bin", 0x0000, 0x080000, CRC(2e729642) SHA1(c8dcdce52f930b3fa894c46907691a28a5499a16) )
#define sc_hellb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR1402 HELLS BELLS SOUNDS11 */ \
	ROM_LOAD( "95008341.bin", 0x0000, 0x100000, CRC(486e5395) SHA1(0ad68f271f4839d50a790b7f4427e1f1f1933bd4) )
#define sc_leg_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008093.bin", 0x000000, 0x100000, CRC(86e27465) SHA1(1d27358fe795286676a8031382387ff4225f7118) ) \
	ROM_LOAD( "95008094.bin", 0x100000, 0x100000, CRC(08909b00) SHA1(3e5b3da186036d7fe67ed2739de6fba79d4a978a) )
#define sc_legcb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008197.bin", 0x000000, 0x100000, CRC(234ff677) SHA1(571c2fa2b5a24d07e90e43061d9947f64874e482) ) \
	ROM_LOAD( "95008198.bin", 0x100000, 0x100000, CRC(4b03df47) SHA1(13c24cc90a618ebc7c150ab3694a1b787fb049d6) )
#define sc_luckb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )  \
	ROM_LOAD( "95008167.bin", 0x000000, 0x080000, CRC(13aaf063) SHA1(70ab005c867c0c8f63b16722f4fc3ba0c935a96f) ) /* lucky balls */ \
	ROM_LOAD( "95008168.bin", 0x080000, 0x080000, CRC(71aebb68) SHA1(479b3915552be029d459d3f05ccf668c21f05554) )
#define sc_mww_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008593.bin", 0x0000, 0x06a118, CRC(e4aee21b) SHA1(bda104a05bd2c0d4506f389c54e8842dc20d84b0) ) \
		\
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890162.bin", 0x0000, 0x5000, CRC(acf9118a) SHA1(b2c9073f0496de62d66edbc1da5546b9239ac1ba) ) \
	ROM_LOAD( "95890163.bin", 0x0000, 0x5000, CRC(b02fe136) SHA1(ac52b411f8117564a8dae25b19c9395e9030bfa0) ) \
	ROM_LOAD( "95890164.bin", 0x0000, 0x5000, CRC(7375c3a6) SHA1(8e5340e201261ccd8b47334f5d23e8adb33d031f) ) \
	ROM_LOAD( "95890387.bin", 0x0000, 0x5000, CRC(7e8703c6) SHA1(139dab5081ce9bdfade8c2a188562d98a15f8bb2) ) \
	ROM_LOAD( "95890388.bin", 0x0000, 0x5000, CRC(6251f37a) SHA1(3c8beafa021b6f4d32bd46f3fd755a7f877414ea) ) \
	ROM_LOAD( "95890389.bin", 0x0000, 0x5000, CRC(a10bd1ea) SHA1(e575a7cd0d40a7282aee2b7cd9f4801408ff846d) )
#define sc_manic_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008684.bin", 0x000000, 0x100000, CRC(52b7d26e) SHA1(ba6629dbad8d00c132c0ddf6a8a41ddc99231c75) ) \
	ROM_LOAD( "95008685.bin", 0x100000, 0x100000, CRC(dc9717c0) SHA1(27234bb7c7e7bd1f395972ce2958d55e84005313) )
#define QPS_PACP_SOUNDS \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* PACP SOUNDS */ \
	ROM_LOAD( "97000000.evn", 0x000000, 0x080000, CRC(5b13fe7b) SHA1(1bd32e577914ab4e3bc3282261f8c3cdf015b85d) ) /* pacp */ \
	ROM_LOAD( "97000000.odd", 0x080000, 0x080000, CRC(8bab1c78) SHA1(ddc915a8c56473ba4d67d8c62c66105dd622b593) )
#define PACMAN_SOUNDS \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* PACMAN SOUNDS */ \
	ROM_LOAD( "95004183", 0x000000, 0x080000, CRC(4b28f610) SHA1(fff01c890a8c109bb4b522ee2391c15abdc2758c) ) \
	ROM_LOAD( "95004184", 0x080000, 0x080000, CRC(bee11fdd) SHA1(b5ce97108812e296c92a000444c1fb7a11286de4) )

#define sc_paccs_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004199.lo", 0x000000, 0x080000, CRC(3a9605c8) SHA1(ce1c94fe26eac9e145e94539f62f2bde740e5b9a) ) \
	ROM_LOAD( "95004199.hi", 0x080000, 0x080000, CRC(0ecfc531) SHA1(15e20eedf4b7d9102c40834612d111559b4dcbca) )
#define sc_paccl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008145.bin", 0x0000, 0x100000, CRC(e2ec54f8) SHA1(bb1c40b13151ed1e3c7ba603506701457392bb8b) ) \
	ROM_LOAD( "pacmancsnd.bin", 0x0000, 0x0bbb33, CRC(c505aa18) SHA1(a99bd1c4101269e2eb2b6becf210d9991fee1da1) ) /* identical but cut at 0xff bytes */
#define sc_pacpl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008049", 0x0000, 0x0ff7f5, CRC(efb3c1aa) SHA1(345ad862a329eedf7c016ddd809c8b60185d9962) )
#define sc_pmani_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008201.bin", 0x0000, 0x100000, CRC(4e0358c5) SHA1(b9d8b78c77f87eebb9408a4ea1b9fd3a64ae724d) )
#define sc_polem_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "pole_p.sn1", 0x000000, 0x080000, CRC(cbb7b019) SHA1(1404ff5fe2c3e54370a79ea141617e58e56217e9) ) \
	ROM_LOAD( "pole_p.sn2", 0x080000, 0x080000, CRC(95d9939b) SHA1(29d03f2f2d33a807df002271e14b614bae3d10e4) )
#define sc_polen_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "polesnd1.bin", 0x000000, 0x080000, CRC(09f720e3) SHA1(1a5c581b16b974e949679c3d71696984755f3c7c) ) \
	ROM_LOAD( "polesnd2.bin", 0x080000, 0x080000, CRC(80c2702b) SHA1(6c7b1d535d8f1eeb25a1a84ccf9e97b3e453af6d) )

#define PR3082_C_POTS_OF_GOLD_SOUNDS11 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008750.bin", 0x000000, 0x100000, CRC(25a94ab7) SHA1(9ee3a1acb2734a5663d925fa050c15d154f79678) ) \
	ROM_LOAD( "95008751.bin", 0x100000, 0x100000, CRC(53c97577) SHA1(5ec3868967073b4ed429fed943fbcd568a85b4f3) )
#define PR000050_POTOFGLD_1 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASEFF) \
	ROM_LOAD( "95008836.bin", 0x000000, 0x100000, CRC(45105fca) SHA1(3c603726bb9f4a01113a225868e864106da9fa35) ) \
	ROM_LOAD( "95008837.bin", 0x100000, 0x100000, CRC(3db8829c) SHA1(e8de259afd2797279154252cfeb6682b482b54a9) )

#define sc_rt_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008203_1.bin", 0x000000, 0x100000, CRC(778a18e5) SHA1(2ad4d0259f08786f50928064a4d345ffa6bb52e2) ) \
	ROM_LOAD( "95008204_2.bin", 0x100000, 0x100000, CRC(ef036383) SHA1(cb26a334fe043c6aba312dc6d3fe91bd93e0cb2f) )

#define sc_sace_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008270.bin", 0x0000, 0x0fde91, CRC(5c87d4ce) SHA1(6fce212a89334a9595411aa2d20bde5e3fae4710) )


#define sc_starp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "star prize snd1.bin", 0x000000, 0x080000, CRC(9f7f7442) SHA1(576e5e92455455ad18ae596a9dae500a17912faa) ) \
	ROM_LOAD( "star prize snd2.bin", 0x080000, 0x080000, CRC(0ae992b5) SHA1(b3a8ba472aa3a6785678a554a7fe789f9f3dc6c0) ) \
	ROM_REGION( 0x400000, "altymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "sprizesn.1", 0x0000, 0x080000, CRC(5a9a9903) SHA1(ac79fedeaa1b37661d0ec28bf6c5b141c7fcc328) )
#define sc_tetri_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008097_1.bin", 0x000000, 0x100000, CRC(01a251eb) SHA1(0cb63736065b97956f02710867ac11609b1f282c) ) \
	ROM_LOAD( "95008098_2.bin", 0x100000, 0x100000, CRC(5ebcfd20) SHA1(be415d965732b3fde47684dacdfe93711182faf4) )
#define sc_trail_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "trailblazer.bin", 0x0000, 0x0f6a6c, CRC(c27b5f9d) SHA1(24ac4ae0bec9a898690dfa2a78f765026d58255b) )
#define sc_ttpie_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008579.bin", 0x0000, 0x100000, CRC(b3741823) SHA1(77890bf89b848fa2222e885aeb51f05f033143ba) )
#define PR6927_CASINO_VMEXICO_SOUNDS11 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004209.lo", 0x00001, 0x080000, CRC(40247425) SHA1(54c8540f9ce45b8b02f6616233b81e33af804f63) ) \
	ROM_LOAD( "95004210.hi", 0x00000, 0x080000, CRC(6f6ce2c8) SHA1(7e9ce336a1307b8290196d58c7419996d0758ab9) )
#define PR3010_VIVA_MEXICO_SOUNDS11 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008719.bin", 0x000000, 0x100000, CRC(f99eafef) SHA1(7a7dedf5bf8292b94ac6da78478441335bfcb66f) ) \
	ROM_LOAD( "95008720.bin", 0x100000, 0x100000, CRC(6419033f) SHA1(3d222696dd9f4f201596fd444a031690cc8c1b0d) )
#define PR6927_CASINO_VMEXICO_SOUNDS21 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004246.bin", 0x000000, 0x080000, CRC(4e5a0143) SHA1(a9e668aceb21671a78b584fd55c21d5501ea1f8a) ) \
	ROM_LOAD( "95004247.bin", 0x080000, 0x080000, CRC(4a35b6a1) SHA1(24e4ed93149b7f6caf785ecaa5a4685585a36f5b) )
#define PR6907_VIVA_MEXICO_SOUNDS11 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "viva_mex.sn1", 0x000000, 0x080000, CRC(ed357575) SHA1(610047e83062fd4a6d23d9b9281a3a6c04d63f7d) ) \
	ROM_LOAD( "viva_mex.sn2", 0x080000, 0x080000, CRC(aab9421d) SHA1(db2941e013597be3fe05a2b002aebdfb0f7a2b41) )
#define PR7132_CLUB_VIVA_SOUNDS11 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008101.bin", 0x000000, 0x100000, CRC(751a433f) SHA1(36aaf1b2425c58bd49671fc6bd61addc33a082f1) ) \
	ROM_LOAD( "95008102.bin", 0x100000, 0x100000, CRC(155ec8ab) SHA1(5f38fb49facab94d041f315178a3d2adf9d95853) )

#define sc_vrgcl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008143.bin", 0x000000, 0x100000, CRC(8bf35d69) SHA1(0a5017d02ba839f2f21402df99e7684ce559d931) ) \
	ROM_LOAD( "95008144.bin", 0x100000, 0x100000, CRC(1ce7f671) SHA1(ccca34515217e72ef1879a3cbad77c7adea3a665) )
#define sc_wldjk_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008999.bin", 0x0000, 0x0b4dd9, CRC(0fa36a2e) SHA1(0168a326fd50c15e8f48293dff45e1411e5044f8) ) \
	ROM_REGION( 0x400000, "pics", 0 ) \
	ROM_LOAD( "95890709.bin", 0x0000, 0x5000, CRC(4b3ba87a) SHA1(6ec85bc6d14825ea0a497e6ee6e38d17c0c7b79f) ) \
	ROM_LOAD( "95890710.bin", 0x0000, 0x5000, CRC(3eadec4e) SHA1(bbb80a6325fd561ca762a0261b68b550285bb8f6) ) \
	ROM_LOAD( "95890711.bin", 0x0000, 0x5000, CRC(a0172012) SHA1(07ceb089ec88801700ea12f52e4cc49e8c1d5d36) )
#define sc_bsp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008725.bin", 0x000000, 0x100000, CRC(e0a46426) SHA1(f6bc41c48ec1c4f113968a6ccc7a7dc81a7674b2) ) \
	ROM_LOAD( "95008726.bin", 0x100000, 0x100000, CRC(f01062bb) SHA1(a75937e85010ff4da01277336ad37bcbb8d0ba9f) )
#define sc_chain_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008318.bin", 0x0000, 0x100000, CRC(d4fb4702) SHA1(b6cdeb8e34d081a403d5918ec95e2eb387102538) )
#define sc_clown_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008552.bin", 0x0000, 0x100000, CRC(89173513) SHA1(7c332b6c14725897b0ae1ed33d38a384eae9cfdd) )


#define sc_bb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008350.bin", 0x0000, 0x0e3be3, CRC(b19e617c) SHA1(46eb761ac44fef0ff0f1731b098b067ce6843461) )
#define sc_bbclb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008383.bin", 0x0000, 0x0f350f, CRC(a93300db) SHA1(4f2864cc71b3fe7ac1b323dfd226c18be83d301f) )
#define sc_botn_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008484.bin", 0x0000, 0x0f8e5a, CRC(83e34402) SHA1(e90c3ef784dfce7df2d60b06bc84ec9a21ff9a12) )
#define sc_brkfs_others \
	ROM_REGION( 0x200000, "ymz", 0 ) \
	ROM_LOAD( "95008013.lo", 0x000000, 0x100000, CRC(286e59da) SHA1(e43901f4ad9fc7f083cbb7ef5cd7e4ad6289833b) ) \
	ROM_LOAD( "95008014.hi", 0x100000, 0x100000, CRC(1b9936ad) SHA1(0c98618d1ca30fa4f9913a5214a5f431e520917c) )

#define sc_bulcl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008547.bin", 0x000000, 0x100000, CRC(33d997ba) SHA1(22c28360757bad350907b145e18a8e438d68f2b1) ) \
	ROM_LOAD( "95008548.bin", 0x100000, 0x100000, CRC(896bae80) SHA1(264127b6d22c048a0a54e7a63433c3aed6f053e2) )
#define sc_bankb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008350.bin", 0x0000, 0x0e3be3, CRC(b19e617c) SHA1(46eb761ac44fef0ff0f1731b098b067ce6843461) )
#define sc_bed_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008315.bin", 0x0000, 0x100000, CRC(5098077e) SHA1(8ecba67e3585dc7851f8bacb7c5235959f883143) )
#define sc_captn_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008139.bin", 0x0000, 0x0c076f, CRC(8c87abf8) SHA1(2dfdf9202378723db267ed9d8f2abd076b5214d6) )
#define sc_cashn_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004292.sn1", 0x000000, 0x080000, CRC(e7efab30) SHA1(b2587c14e427fdb51cb5c96e5e017c69f5c134e8) ) \
	ROM_LOAD( "95004293.sn2", 0x080000, 0x080000, CRC(89584d87) SHA1(935b6e873342aefd1c39bb474c6b780799a0e049) )
#define sc_cashm_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008123_1.bin", 0x000000, 0x100000, CRC(9f1fc98b) SHA1(9233ef6a82f8b23066f959957a7561fb33120d46) ) \
	ROM_LOAD( "95008124_2.bin", 0x100000, 0x100000, CRC(4795c128) SHA1(7b55b39f335b81f6a895ed9beb123464e10a48ee) )


#define sc_cbaz_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008317.bin", 0x0000, 0x100000, CRC(05ef55b5) SHA1(5130b9243647b9724998600b5f2ef2bbe7b5b1e5) )
#define sc_ccc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004282.snd", 0x0000, 0x080000, CRC(e17fb009) SHA1(27fa336c8576bd4a2414f4d71857e9113102cb3d) )
#define sc_ccogs_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008361.bin", 0x0000, 0x0ff54f, CRC(d1cc7b58) SHA1(32328e0e56bbf65e91e32de8802edce38f0abc65) )

#define sc_cclas_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004250.bin", 0x000000, 0x080000, CRC(5250f18b) SHA1(bbdc556df845753f9ce15dd7dfb2347539dfa00f) ) \
	ROM_LOAD( "95004251.bin", 0x080000, 0x080000, CRC(7a0a7876) SHA1(f172c8a1fbe214f201e6bf671c2f0534b6d9d395) )
#define sc_cjcl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008435.bin", 0x0000, 0x100000, CRC(f0b58825) SHA1(124132a510e7473ade3d03fb63f24f120d3a08e4) )
#define sc_ctlcl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008181.bin", 0x000000, 0x100000, CRC(9eb3e51e) SHA1(95e90e9574cba0526e495a8b17150a5081c13df8) ) \
	ROM_LOAD( "95008182.bin", 0x100000, 0x100000, CRC(5d31955a) SHA1(49dbb4f3efc7e03d1763abb3c6db21c81e961735) )
#define PR6923_CRAZY_FRUITS_SOUNDS11 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* PR6923 CRAZY FRUITS SOUNDS11 */ \
	ROM_LOAD( "ccfsnd1.bin", 0x000000, 0x080000, CRC(d389aeb3) SHA1(855f3d4bba4922026b1fa963e60c5e58556739c3) ) \
	ROM_LOAD( "ccfsnd2.bin", 0x080000, 0x080000, CRC(6fbda954) SHA1(812c2c96a9e750daf5ac4a878c63847bfd5a2593) )

#define sc_cfcla_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* PR1396 CLSIC CRAZY FRUITS SND11 */ \
	ROM_LOAD( "95008316.bin", 0x00000, 0x100000, CRC(3b08ccac) SHA1(d71805a424b1af3d54406374a0d1ca95c4bc594c) )
#define PR1430_CRAZY_F_SIT_D_SOUNDS11 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008381.bin", 0x00000, 0x100000, CRC(78c97785) SHA1(5ac40b2850c25699f65ee08c173d194ddb4a3846) )
#define sc_cfclb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004144.lo", 0x000000, 0x080000, CRC(9d861825) SHA1(4b8776a014e31f5041f7e172ae69cb172f42fae5) ) \
	ROM_LOAD( "95004145.hi", 0x080000, 0x080000, CRC(9570fbbc) SHA1(34bdca9ef125e5304b238dd8f1421a888c9ba33e) )
#define sc_cfdu_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008184.bin", 0x0000, 0x100000, CRC(dc98ec1b) SHA1(6b600d7790bb0a90c5309d7e7684e4cca26c5a50) )
#define sc_cfgcl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008577.bin", 0x0000, 0x100000, CRC(f18f1116) SHA1(ba47360e7b489c1bcded786dd1efe518b62b039e) )
#define sc_crcp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008743.bin", 0x0000, 0x100000, CRC(d44e91bf) SHA1(f42a8c8b0ee5f233cddb4b6084a37e45e2490dd9) )
#define sc_crcc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004120.lo", 0x000000, 0x080000, CRC(770a9c0f) SHA1(2d06e32b1d07bc9dc51f39f9ba22c9fe8a678ef3) ) \
	ROM_LOAD( "95004121.hi", 0x080000, 0x080000, CRC(239f389c) SHA1(75d6f9f500aab5f114f8b86c4ca1f8dce6ea2ca4) )

#define sc_crcc_matrix \
	ROM_REGION( 0x400000, "dm01:matrix", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95000611.p1", 0x0000, 0x010000, CRC(3f40a2c9) SHA1(f73731171c56add1329f3a9d2f84303311d87884) )


#define sc_crgc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008323.bin", 0x0000, 0x100000, CRC(461dd238) SHA1(cab717b285fb217bf47ebe07bd6e7737cf0517c9) )
#define sc_crzky_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008242.bin", 0x0000, 0x100000, CRC(7d5c1356) SHA1(efab297024650c95a0abb1296b2ebabd09b299a8) )
#define sc_crzgn_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004248.lo", 0x000000, 0x080000, CRC(4fff74c5) SHA1(2704a15e3afd73848c8f3963920f606310b86963) ) \
	ROM_LOAD( "95004249.hi", 0x080000, 0x080000, CRC(2c8cdeb1) SHA1(771bcc907be91be2d178b87a76b72bd3cc07fe30) )
#define sc_cvani_others \
	ROM_REGION( 0x400000, "ymz", 0 ) \
	ROM_LOAD( "95008237.bin", 0x0000, 0x0fdb37, CRC(ce0a3555) SHA1(7ba0c53709236d41f9e73b3a5151174ca2bf3fae) )
#define sc_cvclb_others \
	ROM_REGION( 0x400000, "ymz", 0 ) \
	ROM_LOAD( "95008345.bin", 0x0000, 0x0fdb37, CRC(1e01057c) SHA1(f54cd2b59b1b4901b972b8b33c85defc975b0fac) )
#define sc_druby_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008423.bin", 0x0000, 0x100000, CRC(4cab20ee) SHA1(e27221a94e54db340eaf7fc30e722b354c85686d) ) /* triple casino */
#define sc_darw_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008214.bin", 0x0000, 0x0e7add, CRC(423575e6) SHA1(52d9a22824b0ee828095f99dcc5209787f5a2c32) )
#define sc_duckq_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008337.bin", 0x0000, 0x0fd2ce, CRC(d452995e) SHA1(3a8196f44078a4730ec18f16ce3a68b647205a29) )
#define sc_dyna_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008209.bin", 0x0000, 0x100000, CRC(7845c5e7) SHA1(14cd505e11149b6bdfa0e8a92236f3229f7a2a0e) )
#define sc_easy_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008195.bin", 0x000000, 0x100000, CRC(e032e67a) SHA1(062f48bd2c38b51ffba8cda7860bb37abad40c71) ) \
	ROM_LOAD( "95008196.bin", 0x100000, 0x100000, CRC(4a23e184) SHA1(2d1c1d92c9cdccd95ca4f466a2d7765def7d990b) )

#define PR1326_CAS_EASY_STREAK_SOUNDS11 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008288.bin", 0x000000, 0x100000, CRC(da74e2d3) SHA1(a16b3d6c0590b0ccc7b2488a2f23383a332c332a) )
#define sc_fastf_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008272.bin", 0x0000, 0x0fafac, CRC(a9bdc1e2) SHA1(60ff02ff4906d8b0f6392cccb96976b912c61e2d) )

#define sc_fd7th_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "frankie snd1.bin", 0x000000, 0x080000, CRC(5d74001e) SHA1(f157706c0970af9b396c2973774c828ed1c0d275) ) \
	ROM_LOAD( "frankie snd2.bin", 0x080000, 0x080000, CRC(8863c8e6) SHA1(0e852155cb2d50d8f0b892c055b37e9955fd8da4) ) \
	\
	ROM_REGION( 0x400000, "altymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "frdesnd1", 0x0000, 0x080000, CRC(deb93ffa) SHA1(5a549b6bde6f92561952584e3184c1985903464f) )

#define sc_frenz_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008052.bin", 0x0000, 0x100000, CRC(36b422b2) SHA1(1ee085393922b46588a604c5e88ad454357711bb) )
#define sc_ftopi_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "frtsnd.bin", 0x0000, 0x0a6326, CRC(f718f2f0) SHA1(eccedf4eda8eda34633b917165e063d2fbb03abf) )
#define sc_ffru_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "ff_snd.bin", 0x0000, 0x0e54bf, CRC(9d1b711d) SHA1(29386915460b8be92fc5f79177efaf05b7f613cd) ) \
	ROM_LOAD( "ff_snd1.bin", 0x000000, 0x080000, CRC(3f0cb386) SHA1(a400177b1e3b986ba5ddfa5f549180b006ced5a2) ) \
	ROM_LOAD( "ff_snd2.bin", 0x080000, 0x080000, CRC(88d7cc1a) SHA1(7cb45ccc41f0c4a842595598f6f30b78e6c42480) )

// the set header says it wants FEVER SOUNDS11, but it accept FEVER SOUNDS12, production error?
#define sc_fever_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* PR1007 FEVER SOUNDS12 */ \
	ROM_LOAD( "95008137.bin", 0x000000, 0x100000, CRC(f74f3916) SHA1(659b160db41e46a53149688f8677e73a78f22e63) ) \
	ROM_LOAD( "95008138.bin", 0x100000, 0x100000, CRC(68563cdd) SHA1(df92dadecf9242bfa147e3134039266e9016faea) )
#define sc_fever7157_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* 95004239 FEVER PR7157 */ \
	ROM_LOAD( "95004239.sn1", 0x000000, 0x080000, CRC(12c4d8eb) SHA1(a404bce7eb1a1148cba4a4d8326ea36e31e881b4) ) \
	ROM_LOAD( "95004240.sn2", 0x080000, 0x080000, CRC(a990a2c4) SHA1(2630dc0408359f32d05ecfd5bcc81789854e5229) )

#define sc_fevnx_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004318.sn1", 0x000000, 0x080000, CRC(bb5d74bd) SHA1(104dd8b2faedce3382533c755ca05d03ec2bc50a) ) \
	ROM_LOAD( "95004319.sn2", 0x080000, 0x080000, CRC(73dc297e) SHA1(a826e7d0b88b9e01ceab21a2a92f69f15b402acb) )
#define sc_fpitc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008091.bin", 0x000000, 0x100000, CRC(81a28437) SHA1(99b9e5c04c2ab7e9aba0b9b8a0bacee424a079de) ) \
	ROM_LOAD( "95008092.bin", 0x100000, 0x100000, CRC(0987737f) SHA1(bab6bf1ffd3df5465c87996795b1c25f5e863beb) )
#define sc_fcc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004103.lo", 0x000000, 0x080000, CRC(9926f103) SHA1(f29a0d25eebd0a0990cd165116b425b795ed62e0) ) \
	ROM_LOAD( "95004104.hi", 0x080000, 0x080000, CRC(bec75a23) SHA1(cdf895081ebf3afb52d5bfaab29f713800c85fc9) )
#define sc_fcc_matrix \
	ROM_REGION( 0x200000, "dm01:matrix", ROMREGION_ERASEFF )\
	ROM_LOAD( "club-firecracker_mtx_ass.bin", 0x0000, 0x010000, CRC(c23ffee9) SHA1(b4f2542e8ed0b282a439e523baa6cd43c5b2cb50) )
#define sc_frsu_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004229.bin", 0x000000, 0x080000, CRC(fe94f724) SHA1(6cebd17d277f1fa88982158d5ac9422f6f16bc3b) ) \
	ROM_LOAD( "95004230.bin", 0x080000, 0x080000, CRC(4b0a6a18) SHA1(6df706b4431cc97318df612c9533c34085cd341d) )
#define sc_fullt_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008449.bin", 0x0000, 0x0fde7d, CRC(9e3ce927) SHA1(00ce43a0ca94e81ac90f53c647e40ac6cb454937) )

#define sc_greed_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008325.bin", 0x0000, 0x100000, CRC(82356c95) SHA1(bbf9dc2e4ca20a35a8bc581e322956e17f220022) )
#define sc_gslam_others \
	ROM_REGION( 0x200000, "ymz", 0 ) \
	ROM_LOAD( "95004169.bin", 0x000000, 0x080000, CRC(d2293b2f) SHA1(e3eed38a15885c5a6d63fc79d95f77c5225e9168) ) \
	ROM_LOAD( "95004170.bin", 0x080000, 0x080000, CRC(1da0db8f) SHA1(b3c37b5e02efe7267556d8603c362298ed55bb88) )
#define sc_heatw_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004287.snd", 0x0000, 0x080000, CRC(7a86c853) SHA1(60a5582f43cda30e9c48a1207ee1a4cafdce0276) )
#define sc_helrd_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004326.snd", 0x0000, 0x080000, CRC(193851ca) SHA1(de71622b6d50d039bd625189303191d316bdc671) )
#define sc_helrs_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004326.snd", 0x0000, 0x080000, CRC(193851ca) SHA1(de71622b6d50d039bd625189303191d316bdc671) )

#define sc_hi5_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008107.bin", 0x000000, 0x100000, CRC(f2b5a284) SHA1(c2b427aa59a233eedefff55d3afdc120f486af83) ) \
	ROM_LOAD( "95008108.bin", 0x100000, 0x100000, CRC(c8bd435d) SHA1(cc78030ae92424db3ba572f355d12db7f9291bd7) )
#define sc_sprng_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008276.bin", 0x0000, 0x0f7b99, CRC(e13380bd) SHA1(f4ec3f2d4d8f20e82324fda89e5383a9689bf02a) )
#define sc_hilo_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008177.bin", 0x000000, 0x100000, CRC(4dad5ea3) SHA1(efa226af2bae5bab25deb3507a635412b0f031e8) ) \
	ROM_LOAD( "95008178.bin", 0x100000, 0x100000, CRC(3a59e427) SHA1(7d530426a57dd9651aef51730b0b8a5324f81024) )
#define sc_hitsh_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008239.bin", 0x0000, 0x100000, CRC(cc12346b) SHA1(dd3656a3024c3d26348482c3aa04e0f6638053f7) )
#define sc_holyw_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008071.bin", 0x000000, 0x100000, CRC(005c5847) SHA1(9d8a801e70f5b1c07d46a4f6e9fab22adf5a9415) ) \
	ROM_LOAD( "95008072.bin", 0x100000, 0x100000, CRC(78756706) SHA1(56b744ec5cb431b30c13bc748d323ebe14727546) )
#define sc_h6cl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008576.bin", 0x0000, 0x100000, CRC(15d77734) SHA1(db314df1dc558ce66d3c4d62895ab699d36bdf46) )

#define sc_hf_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008163.bin", 0x0000, 0x100000, CRC(0d8abe3c) SHA1(e3dc45ca8aea3113c5884ae3af9216f627ce71ba) )
#define sc_hotpr_others \
	ROM_REGION( 0x800000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "97000034.p1", 0x000000, 0x100000, CRC(f06cb0f1) SHA1(856bdd31a35e93e6f52f88f2ac738cc3fab09c11) ) \
	ROM_LOAD( "97000034.p2", 0x100000, 0x100000, CRC(cce5a2a7) SHA1(4884e8abe2dde81f70ddc13a3a68d690fc097dae) ) \
	ROM_REGION( 0x800000, "others", ROMREGION_ERASE00 ) \
	/* bad dumps of snd1? */ \
	ROM_LOAD( "95008015.lo", 0x00000, 0x800000, CRC(6553f5b9) SHA1(17ab5162f4fd3fa5a56641f0b1cbc31c0fe1a52c) ) \
	ROM_LOAD( "95008016.lo", 0x00000, 0x800000, CRC(6553f5b9) SHA1(17ab5162f4fd3fa5a56641f0b1cbc31c0fe1a52c) ) \
	/* bad dumps (2nd half of regular roms) */ \
	/* ROM_LOAD( "hot_p.sn1", 0x0000, 0x080000, CRC(c918da0e) SHA1(7eebd41c70be65b1455bafd4087246f41a8b3622) ) */ \
	/* ROM_LOAD( "hot_p.sn2", 0x0000, 0x080000, CRC(31fa3827) SHA1(aa099bfda63d3627cab2b5f12fe0ec7d8600f90e) ) */
#define sc_ijclb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008888.bin", 0x0000, 0x0fc30c, CRC(71ad9f31) SHA1(40d42bbe7e3d38cab48006a4f0d4d25d2b56a647) ) \
	ROM_REGION( 0x5000, "pics", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95891060.bin", 0x0000, 0x5000, CRC(791ebcf2) SHA1(88298c6d1eec718b84e51aae447ffd74493a27f4) )
#define sc_jack_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008289.bin", 0x0000, 0x0ffb45, CRC(76d2af0f) SHA1(0460ecc4f80cceb6861b8b0ebf59f0e87bf2b38c) ) \
	ROM_LOAD( "jack the kipper sound 1 maz fc64.bin", 0x0000, 0x100000, CRC(f31090f9) SHA1(4e9edfdf6472b4229d4393cb3033134f6b27920b) )
#define sc_jjunc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008284.bin", 0x0000, 0x100000, CRC(f90feeb8) SHA1(672abc79a88b2237fa584d12b9123a2baeec83b8) )
#define sc_jjucl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008395.bin", 0x0000, 0x100000, CRC(619865d6) SHA1(9e76267140709ae2ca09ea0866fc0b27b94d26af) )
#define sc_jiggn_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004179", 0x000000, 0x080000, CRC(fd2521e8) SHA1(58027efa8110004fc4d823dc19d4de115a925711) ) \
	ROM_LOAD( "95004180", 0x080000, 0x080000, CRC(7750b004) SHA1(4c6cb7a5c4b69ca1d65b57271798ac17cd06ce61) )

#define sc_jolly_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008254.bin", 0x0000, 0x0f9355, CRC(4742beac) SHA1(2f060a5f54719fdf5aea1077d8d5de6534b41f0c) )
#define sc_juicy_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008216.bin", 0x000000, 0x100000, CRC(1fcf52e3) SHA1(0e585678cb928e107343ebc64f77434ac6a62555) ) \
	ROM_LOAD( "95008217.bin", 0x100000, 0x100000, CRC(995f77f6) SHA1(ac964bc58490448cd3b5a1ed0c7636c2354c8c7d) )

#define sc_jjf_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004085.lo", 0x000000, 0x080000, CRC(1b1f3455) SHA1(8733193640b907cf0aae3bc474dd4f1766c6e74c) ) \
	ROM_LOAD( "95004086.hi", 0x080000, 0x080000, CRC(5b678da4) SHA1(05c3760b35e77ea70401e47762dcbf0cc779ea3c) )
#define sc_kalei_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008444.bin", 0x0000, 0x0f8d34, CRC(a6a6c950) SHA1(97b9859a8795a5890156ce68f71b3f67707efdd2) )

#define sc_lasv_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004261.snd", 0x0000, 0x080000, CRC(a2fbd1f1) SHA1(7ae7108235f4b9758282506558054b056d92abed) )
#define sc_lined_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004241.bin", 0x000000, 0x080000, CRC(e524fd19) SHA1(af0bcf9cf75592beb928f78a915875f3e3ecedac) ) \
	ROM_LOAD( "95004242.bin", 0x080000, 0x080000, CRC(6d86611e) SHA1(2ec6a6745446502a77c10c487b75b59be1fc266e) )

#define sc_ldvcl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008897.bin", 0x0000, 0x0ff4f6, CRC(3b459b50) SHA1(ba4aa9bcf42cd95f366fbe51c343680cd3fbcd47) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95891079.bin", 0x0000, 0x5000, CRC(378bb888) SHA1(054cdcad83a3909c8c17852f3848c7a7c4daae31) )
#define sc4lockb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008223.bin", 0x0000, 0x100000, CRC(1b938782) SHA1(501567017e5097c616b90623b16e503b5ab941fd) )
#define sc_lkbcl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008285.bin", 0x0000, 0x100000, CRC(c6a5033b) SHA1(9c7abea9052f606dd501e359a4fc4b69860c5269) )
#define sc_lir_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008103.bin", 0x000000, 0x100000, CRC(74ec1e6c) SHA1(1636192a63afcb060ee9453d24d7deb3a6776f51) ) \
	ROM_LOAD( "95008104.bin", 0x100000, 0x100000, CRC(b11bd959) SHA1(1739035495a14fedcbc67b724e7595693425d15f) )
#define sc_miljo_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004305.sn1", 0x000000, 0x080000, CRC(1fffefe8) SHA1(e4dd8ee3ac82c598b50fe49afeddf11bcca96581) ) \
	ROM_LOAD( "95004306.sn2", 0x080000, 0x080000, CRC(afc501d4) SHA1(9a28381a7c1c047d6e7bb70736d2c8308c8911a3) )
#define sc_milja_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004305.sn1", 0x000000, 0x080000, CRC(1fffefe8) SHA1(e4dd8ee3ac82c598b50fe49afeddf11bcca96581) ) \
	ROM_LOAD( "95004306.sn2", 0x080000, 0x080000, CRC(afc501d4) SHA1(9a28381a7c1c047d6e7bb70736d2c8308c8911a3) )

#define PR1132_MONEY_GO_ROUND_SOUNDS11 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "snd_mgo.p1", 0x0000, 0x100000, CRC(b2f8dd11) SHA1(2db747a0c422d7dd18972f80ba94570c69564c9b) )

#define PR1132_MONEY_GO_ROUND_SOUNDS11_ALT \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008212.bin", 0x0000, 0x100000, CRC(1d0fffb3) SHA1(da7e10479b1ba9e67af94feaa20702bae687168f) )
#define sc_mspid_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008278.bin", 0x0000, 0x100000, CRC(8fc20733) SHA1(6ede5578fa11cb3322291958d6e1ef8f6cd99da0) )
#define sc_msclb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008191.bin", 0x0000, 0x100000, CRC(4d392e0c) SHA1(798ba07fdb0bebe5367f58b1e13226827153e0e8) )
#define sc_mtb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "mtb_snd1.bin", 0x000000, 0x080000, CRC(4abca575) SHA1(8f0e3fd169cbf0876368dac335bf81ab0601c923) ) \
	ROM_LOAD( "mtb_snd2.bin", 0x080000, 0x080000, CRC(034a49dc) SHA1(aac70d867856f13f5948e81ed3ca7f2746edf134) )
#define sc_mtbcl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008009.lo", 0x000000, 0x100000, CRC(b9334137) SHA1(e7481688e18b56bddee4a2eff219f83ac5180082) ) \
	ROM_LOAD( "95008010.hi", 0x100000, 0x100000, CRC(cb197b7e) SHA1(56fb416a98bc2875df11024518babf4a07418a79) )

#define sc_magci_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008480.bin", 0x0000, 0x0c4846, CRC(025a1417) SHA1(6f522e71b4707829c98f77551f7cd4f2910f15b5) )
#define sc_magic_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008340.bin", 0x0000, 0x0ff5d7, CRC(1371b0d9) SHA1(aa8c23a86cbd5e8274bb035c1132da11d71fd2f8) )
#define sc_maxim_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008151.bin", 0x0000, 0x184af2, CRC(775d8471) SHA1(d784e25f52e6df18da9eabf93e6b50de5217c374) )
#define sc_monob_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* both of these are the same, one is just padded */ \
	ROM_LOAD( "95008359.bin", 0x0000, 0x0e89fa, CRC(763e4367) SHA1(c44b70f8336c2d6c00cd0dde9b6ca014667c59a6) ) \
	ROM_LOAD( "monopoly95008359_sound1.bin", 0x0000, 0x100000, CRC(93f005f2) SHA1(d2351982d6352d9793efac617ac9fb9569f229c6) )

#define sc_mou_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008364.bin", 0x0000, 0x0e98ac, CRC(47aae23d) SHA1(0964a4f68c3688ebd65daf317b17e1341fcd3ceb) )
#define sc_nmtj_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008472.bin", 0x0000, 0x0fe798, CRC(a522157a) SHA1(f1fea963579cb8caddd5fcbfa36e33dc35c6e2a0) )
#define sc_mclb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "cwow sounds", 0x000000, 0x100000, NO_DUMP )
#define sc_mdlx_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008514.bin", 0x0000, 0x0ed840, CRC(2608cbde) SHA1(4831570fbc825592781e6473173d6ec911a0054d) )

#define sc_mont_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004314.bin", 0x0000, 0x0ccf04, CRC(0faba601) SHA1(1f21e39c2a9a18dd74292e68456c725ec2631aac) ) /* lotech sounds? */

#define sc_nudit_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95404303.bin", 0x000000, 0x080000, CRC(adb71c93) SHA1(e91165b0280da9f79415b1cd7bb2ff8c1ea974fe) ) \
	ROM_LOAD( "95404304.bin", 0x080000, 0x080000, CRC(96190422) SHA1(0e3192d959f1ea4296474684f04847c50a983400) )
#define sc_pen1_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "pub-en.s1", 0x000000, 0x080000, CRC(7e3f4295) SHA1(b7a2c538d79663a3d21f89311195619158fa7197) ) /* need testing */ \
	ROM_LOAD( "pub-en.s2", 0x080000, 0x080000, CRC(637d3c0f) SHA1(193964efc28e56b05f39099a696dd3e9119b80dd) ) \
	\
	ROM_REGION( 0x400000, "dm01:matrix", 0 ) \
	/* I don't think the SC4 version uses a DMD */ \
	ROM_LOAD( "pbemydot", 0x0000, 0x010000, CRC(b056d3d4) SHA1(6c1dbc6fcb4761c25f9cc8123e9f0fe791488c19) ) /* DMD rom? Possibly SC2 DM01 */
#define sc_oyf_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008301.bin", 0x0000, 0x100000, CRC(0e042883) SHA1(2e73e5a49bcc525edfddf56e0d4ef5137116eec2) )
#define sc_opses_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008211.bin", 0x0000, 0x100000, CRC(535fbad0) SHA1(25804a47e20dcfe364f49195da0ab057bce31266) )


#define sc_party_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008113.bin", 0x000000, 0x100000, CRC(7e54f87c) SHA1(b64e4da7c088ba2f328d6adcfd0a3824d98a5f31) ) \
	ROM_LOAD( "95008114.bin", 0x100000, 0x100000, CRC(594fef43) SHA1(8549d372ffb0194823afc11992d8a16f6caed2ff) )
#define sc_paytm_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004224.sn1", 0x000000, 0x080000, CRC(74382d29) SHA1(bd0323b7208e9b03e2d8468f7d4603fb4a3fe394) ) \
	ROM_LOAD( "95004225.sn2", 0x080000, 0x080000, CRC(23efa88d) SHA1(af52ec3dcb45a2ea26eaa273c783134402105168) )
#define SC4_95004316_CAS_PHAR_GOLD_PR1261 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004316.snd", 0x000000, 0x080000, CRC(94a9ec9c) SHA1(bab1a431e2857b9db9fa3f3ef28f150e19c4ae50) ) /* casino */
#define sc_pgold_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008141.bin", 0x000000, 0x100000, CRC(e1b214dc) SHA1(811f6ec42961496f8cd9fb3434ab6a07457ca61a) ) \
	ROM_LOAD( "95008142.bin", 0x100000, 0x100000, CRC(faa84d52) SHA1(f5139df5e9ff95fab3c01ac2b1aa44b1d939bf7a) )
#define sc_polic_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008327.bin", 0x0000, 0x0d24fe, CRC(767251af) SHA1(9cc9f4f2c6f6762a1929b845521549256e9b9f4f) )
#define sc_potsh_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008431.bin", 0x0000, 0x0ffeaf, CRC(400843fd) SHA1(35a0195b120e61c40e8f5e193b44f9fcb5b7d99d) )

#define sc_pwrsg_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008257.bin", 0x0000, 0x0dc608, CRC(ac648ab0) SHA1(58c919be129388ca9c34cad177520175ef08cfaa) )

#define sc_pir_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008264.bin", 0x0000, 0x100000, CRC(c3301cf1) SHA1(59e3c5c8ab7be1901800d493fe98e72dd24c181c) )
#define sc_ppclb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008934.bin", 0x000000, 0x100000, CRC(87290522) SHA1(27deddd66b0ac9a0f4fe526f0eda7f6b2e73a8b6) ) \
	ROM_LOAD( "95008935.bin", 0x100000, 0x100000, CRC(7517053c) SHA1(d83019c52b8c78539359e9277864717c76fae0ce) )
#define sc_pipe_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "pipingsnd.bin", 0x0000, 0x0c50d9, CRC(ff240faa) SHA1(f0961b3207c0c8779479c773d1453ac4ff376bd2) )
#define sc_plumb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008430.bin", 0x0000, 0x0c690f, CRC(29059625) SHA1(4e46f794726b778e03f67ab3ec3cd4cb71be0b64) )


#define sc_qmodo_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "97000080.bin", 0x0000, 0x0ea2e7, CRC(060bf37a) SHA1(732fa84ed0145f931557e3fef66276370dde6884) )
#define sc_quidr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008269.bin", 0x0000, 0x0e5ee2, CRC(b61e664a) SHA1(9ee423015b5532cc4c06ddb76846800fa325074c) )


#define sc_redad_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004266.snd", 0x0000, 0x080000, CRC(457fffbf) SHA1(ede2adc836ba4904cdf3d1ed825f9128c27b2b58) )
#define sc_redal_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004266.snd", 0x0000, 0x080000, CRC(457fffbf) SHA1(ede2adc836ba4904cdf3d1ed825f9128c27b2b58) )
#define sc_rio_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004309.snd", 0x0000, 0x080000, CRC(60eb25fc) SHA1(66f3ac8bc1be99eda07c71f63ebc818014ef7800) )
#define sc_rvlnx_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004320.snd", 0x0000, 0x080000, CRC(b20c0557) SHA1(f61df4a80751571caf4d44add753b5477c13b7f0) )
#define sc_rvl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004259.sn1", 0x000000, 0x080000, CRC(c26ed994) SHA1(da336ab76c1c148906c734d7239b2b83a05ec083) ) \
	ROM_LOAD( "95004260.sn2", 0x080000, 0x080000, CRC(1a32d1bd) SHA1(e88ba7b8b353ea168a689e1d2be8c753a56f8f1d) )
#define sc_rogds_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008256.bin", 0x0000, 0x0fafa4, CRC(6d70436f) SHA1(adf7d18d9fcbb216ee461183951c1ab2972b8ef7) )

#define sc_royle_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008207.bin", 0x000000, 0x100000, CRC(269f845f) SHA1(eaddeaebdafd3f237b6af5c3539da407a6782210) ) \
	ROM_LOAD( "95008208.bin", 0x100000, 0x100000, CRC(07d4d9fa) SHA1(841fead29de19da0436f937614de50e1f07ebc8b) )
#define sc_rbank_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008233.bin", 0x000000, 0x100000, CRC(ebdb6017) SHA1(ca723483f6a7ae6512fa4abc4437c1f8eb606784) ) \
	ROM_LOAD( "95008234.bin", 0x100000, 0x100000, CRC(4b7ac02a) SHA1(29c51906e5ca1e0c9c70db86e3a24f08be06b7e0) )
#define sc_pstat_others \
	ROM_REGION( 0x100000, "ymz", 0 ) \
	ROM_LOAD( "95008294", 0x0000, 0x100000, CRC(0d87f9af) SHA1(7b00cbe3ba69f646fd692bfd6f002bd37801325e) )
#define sc_po8_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008416.bin", 0x0000, 0x0fe66d, CRC(9ae69b6a) SHA1(7bb94d7c941c64c1ea53e391b3f66a6e1b483417) )
#define sc_sidsp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008369.bin", 0x0000, 0x100000, CRC(76ea0074) SHA1(6cc4f320d8e331477569dfc7e961afd30a4ce7b2) )

#define sc_sbust_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008255.bin", 0x0000, 0x0d55db, CRC(feef5ac9) SHA1(83b7de3bd3a1fa3f5ae17f102cc54467d564740f) )
#define sc_strbr_others \
	ROM_REGION( 0x400000, "unk", ROMREGION_ERASE00 ) \
	/* this looks encrypted, or like samples.. I don't think it belongs here, check */ \
	ROM_LOAD16_BYTE( "sbar713.bin", 0x0000, 0x010000, CRC(c7e26d6e) SHA1(1e270d4f5de92083f54cc3fe246b2bc1d2f441b3) ) \
	ROM_LOAD16_BYTE( "sbar714.bin", 0x0000, 0x010000, CRC(57dbca5f) SHA1(22a6fcf1e11dd9ba2b3f73cd7520e2b4a740a893) ) \
	ROM_LOAD16_BYTE( "sbar715.bin", 0x0000, 0x010000, CRC(e9192085) SHA1(a030e81f54338bcb8667c59bad9a6b35a41bbd9f) ) \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004290.snd", 0x0000, 0x080000, CRC(488972c2) SHA1(4cd9bbb6df0fbaf9b8dd93294046c71c82bf4b5b) )
#define sc_strx_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008247.bin", 0x0000, 0x100000, CRC(0817596f) SHA1(250db171d1f7893883ffbcfdd3fd3f6e62524bdc) )
#define sc_s6c_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004095.p1", 0x000000, 0x080000, CRC(d9d7b876) SHA1(fdd9b5f07665b7b7edd198e00d3b005b32d1609c) ) \
	ROM_LOAD( "95004096.p1", 0x080000, 0x080000, CRC(fb8e103f) SHA1(fd8e2c8ae1e459f7426aa6b5de20f91dfae7bec9) )


#define sc_slc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008412.bin", 0x0000, 0x100000, CRC(197edb63) SHA1(f1851029f6870d41d6aaf2df0a49fc54f5c79e4c) )
#define sc_sstep_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "97000120.bin", 0x0000, 0x080000, CRC(d887ca0a) SHA1(b1fd4633ca47fe1d99c9938af48768cd0cf451c6) )
#define sc_solgl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008159.bin", 0x000000, 0x100000, CRC(e6ad55e4) SHA1(4bea205a9e6e2832cd06d0d5c96599f2a83f8fec) ) \
	ROM_LOAD( "95008160.bin", 0x100000, 0x100000, CRC(d52329ca) SHA1(e0feebe0d0ec13cd15cf4af1023246a1a3a9adc7) )

#define sc_spark_others \
	ROM_REGION( 0x200000, "ymz", 0 ) \
	ROM_LOAD( "95008031.bin", 0x000000, 0x100000, CRC(445d3cf7) SHA1(ded74e46b4fb4ef405eb74027a63d301efce1f45) ) \
	ROM_LOAD( "95008032.bin", 0x100000, 0x0fd082, CRC(88781139) SHA1(e8e972032664c314f5263d7acb5a3f0769c7fb2a) )
#define sc_stirc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008117.bin", 0x0000, 0x197791, CRC(9d555ddf) SHA1(753b838fa7f64317b64fe0ae0ee7e6484f2a5af3) )
#define sc_suscl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008865.bin", 0x0000, 0x0f4505, CRC(8e375291) SHA1(d7563adf330023f4fe47c11eb9ef77d593c3eaab) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890751.bin", 0x0000, 0x5000, CRC(57c944b0) SHA1(5f9aadffda6e83780e89c64315c8289a7d364ef6) ) /* does this belong with an sc5 set? */
#define sc_strk_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008492.bin", 0x0000, 0x0fa055, CRC(4f37c766) SHA1(352dc95bb1f0fe31db233ac731618acb3046681e) )
#define sc_taekw_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008283.bin", 0x0000, 0x0fd772, CRC(59a3924c) SHA1(ceba9236db0b26eb79feabc9564eb6dec10c4ab9) )
#define sc_taknt_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008273.bin", 0x0000, 0x100000, CRC(c8695859) SHA1(5998d1f3467e522d4fa5ad6cf75b86ae1760fdd6) )
#define sc_takcl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008314.bin", 0x0000, 0x100000, CRC(64d2a26a) SHA1(f01944363e77bf33b44a06224e44828b065fdee4) )
#define sc_tbana_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008229.bin", 0x0000, 0x100000, CRC(003f9c3d) SHA1(38ea111096e84e77e727fb784992b525767f5b0c) )
#define sc_ticlb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004082.p1", 0x000000, 0x080000, CRC(b7caba0f) SHA1(777afdb6a2f78edad5f4df506eb4cd571f9f357b) ) \
	ROM_LOAD( "95004083.p2", 0x080000, 0x080000, CRC(c0cc21b7) SHA1(d0b22db4c1faeef34b794ac4c31bc9fd386493ea) )
#define sc_ticlb_matrix \
	ROM_REGION( 0x400000, "dm01:matrix", 0 ) \
	ROM_LOAD( "club-treasure-island_mtx_ass.bin", 0x0000, 0x010000, CRC(74f97b29) SHA1(9334bf1e4b4e2bcbbfaa5ae32201ceaab0641d83) ) /* DMD */
#define sc_tri7_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR1328 TRIPLE 7S SOUNDS11 */ \
	ROM_LOAD( "95008308.bin", 0x0000, 0x100000, CRC(87a25bc3) SHA1(bf3d6b86f8f217131c33b681787f4a6fe84b8b8d) )



#define sc_tic2_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* TTTWO SOUNDS */ \
	ROM_LOAD( "95004231_1.bin", 0x000000, 0x080000, CRC(af1512be) SHA1(4196820fa06b5e09f7108894ca89fc43e4ac3da5) ) \
	ROM_LOAD( "95004232_2.bin", 0x080000, 0x080000, CRC(bddddec8) SHA1(3dd7dab9ad3e4475cd10c675bbc5a5fd28cf953e) )
#define sc_tgear_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* GEARS SOUNDS */ \
	ROM_LOAD( "95008063.1", 0x000000, 0x100000, CRC(f87314dc) SHA1(00528243bd58e50ab24dde879f352feec2721019) ) \
	ROM_LOAD( "95008063.2", 0x100000, 0x100000, CRC(5d25d9e8) SHA1(a7f1abcf4bdb5ced2b414d43a13568d13ac334de) )
#define sc_tload_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* LOAD SOUNDS */ \
	ROM_LOAD( "95008362.bin", 0x0000, 0x100000, CRC(fb14b937) SHA1(c8cbdda6184cc3d5b837a3cda105a39cf745bd4d) )
#define sc_trist_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* STR3 SOUNDS */ \
	ROM_LOAD( "95008499.bin", 0x0000, 0x0efbd7, CRC(90710d1f) SHA1(923f6396941b713d3f55bb002c36b0e42063ae4d) )
#define sc_ttomb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR1305 TRES TOMB SOUNDS11 */ \
	ROM_LOAD( "95008287.bin", 0x0000, 0x100000, CRC(9a3f47f9) SHA1(5844f3be0fe4febe04d924afc00f2f55273c15f1) )
#define sc_tridn_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* TRID SOUNDS */ \
	ROM_LOAD( "95008335.bin", 0x0000, 0x0fd5ab, CRC(f0fed8d5) SHA1(dd974c70adfb9a5fb37a069eb736d5ef23b43ddf) )
#define sc_tub_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR1103 TUBULAR BELLS SOUNDS11 */ \
	ROM_LOAD( "95008164.bin", 0x0000, 0x100000, CRC(37fb6004) SHA1(d171223dd4a4162bfb1e63e4af26ea012a2de5ea) )
#define sc_twilt_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* 95004299 TWILIGHT PR1154 */ \
	ROM_LOAD( "95004299.snd", 0x0000, 0x080000, CRC(f4837e10) SHA1(78c4455b1585ca60ed15734dac517f5f1ef4d237) )


#define sc_valnv_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* 7025 VER1 SOUNDS */ \
	ROM_LOAD( "sound1.bin", 0x000000, 0x080000, CRC(cedee498) SHA1(311ab99746cff8f72ec8ffb9ac386a713fc300dd) ) \
	ROM_LOAD( "sound2.bin", 0x080000, 0x080000, CRC(0994b7bb) SHA1(ad79998467fee8fd95c944ea6a33450fcf6d3d56) )
#define sc_valqp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR2040 SOUNDS V1 */ \
	ROM_LOAD( "95004296.sn1", 0x000000, 0x080000, CRC(aecc8138) SHA1(64fd18666c2c753899bff971227b9358c2bf22f1) ) \
	ROM_LOAD( "95004297.sn2", 0x080000, 0x080000, CRC(2c8b698b) SHA1(7b8dd1f2215579b5ad69a2a1f11bfcd04e3d07f9) )


#define sc_wernr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR7027 SOUNDS V11 */ \
	ROM_LOAD( "gw_snd_1.8mb", 0x000000, 0x100000, CRC(990a3471) SHA1(356120558681440cac8252223c31fb9eca2572db) ) \
	ROM_LOAD( "gw_snd_2.8mb", 0x100000, 0x100000, CRC(468431e6) SHA1(90e8dd52b1ce752672b0a02d3cb7715cfa3fb49d) )
#define sc_waw_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* WILD SOUNDS */ \
	ROM_LOAD( "95004292.bin", 0x0000, 0x17ba49, CRC(a5a03ead) SHA1(f1319d0e254b980262e3d404410e6ddbf18057b6) ) \
	\
	/* this is just 0x80000 - 0xfffff of the above */ \
	/* ROM_REGION( 0x400000, "altymz", ROMREGION_ERASE00 ) */ \
	/* ROM_LOAD( "wetsnd1", 0x0000, 0x080000, BAD_DUMP CRC(5fb985b3) SHA1(19f85535b5082546a6098a751a2e2244ed4373ec) ) */

#define sc_winsp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* WINNINGSPINSND */ \
	ROM_LOAD( "95008504.bin", 0x0000, 0x0bd369, CRC(c27f88ca) SHA1(bfc34cc433b68dc6377af7ee464cbdb74a6e0e8d) )

#define sc_wspin_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* WINSPINNERSND */ \
	ROM_LOAD( "95008516.bin", 0x0000, 0x0c35e4, CRC(b8deeea9) SHA1(5bc5f96e5f1b994448d90f05eea2e1c01e17ab66) )
#define sc_wwys_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR1301 WIN W Y S SOUNDS11 */ \
	ROM_LOAD( "95008277.bin", 0x0000, 0x100000, CRC(19051eb6) SHA1(accc9b452dd80e6ca40e79dcc7d4fc6d4f234243) )
#define sc_winxp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* 95004265 WINXPLOSION PR1053 */ \
	ROM_LOAD( "95004265.snd", 0x0000, 0x080000, CRC(1e3e761d) SHA1(81c1e1af72836b25a053cf62c52341a0f8837e3e) )
#define sc_wondw_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR1314 WONDERWH SOUNDS11 */ \
	ROM_LOAD( "95008330.bin", 0x0000, 0x100000, CRC(15e80096) SHA1(da02a8afc83400fdf1ead9dad8a461eb9e911586) )
#define sc_xmark_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR1116 X MARKS T S SOUNDS11 */ \
	ROM_LOAD( "95008263.bin", 0x0000, 0x100000, CRC(43507f43) SHA1(a5eb9e142847a338bf18f4b26da7fe5a2807a027) )
#define sc_xcash_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* 95004321 XTRACASH PR1264 */ \
	ROM_LOAD( "95004321.snd", 0x0000, 0x080000, CRC(97682381) SHA1(b1ef910e7b2e3574576fb6583a088c3de61e6019) )




#define sc_clbtm_matrix \
	ROM_REGION( 0x400000, "dm01:matrix", ROMREGION_ERASE00 ) \
	ROM_LOAD( "clubtempdot.bin", 0x0000, 0x010000, CRC(283d2d9c) SHA1(5b76a13ad674f8a40c270e5dbc61dac04d411d02) ) /* DM01 */ \
	ROM_REGION( 0x400000, "matrixhex", ROMREGION_ERASE00 ) /* can probably be removed, need to verify it matches first tho */ \
	ROM_LOAD( "club-temptation_mtx,ihex,ss.hex", 0x0000, 0x01d0da, CRC(08ebee96) SHA1(2e87d734c966abab1d4a59c9481ebea161f77286) )

#define sc_dcrls_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* DCRZYSND */ \
	ROM_LOAD( "95008482.bin", 0x0000, 0x04c048, CRC(e0b13788) SHA1(e364ccedd31e1fe0560312a28ad6d5cfaac5bfaa) )
#define sc_gcclb_others \
	ROM_REGION( 0x400000, "altymz", ROMREGION_ERASE00 ) \
	/* original files */ \
	/* ROM_LOAD( "95004252.bin", 0x0000, 0x134084, CRC(b05844f8) SHA1(e2a2f3ad69cc9a66ca703e9e07ac008300d4139d) ) */ \
	/* ROM_LOAD( "95004253.bin", 0x0000, 0x134084, CRC(a546da40) SHA1(fead6e4f8dab919f65c48cf04100a7742432b1f4) ) */ \
	/* converted to binary from intel hex */ \
	/* GRAND`CASINO`SOUND */ \
	ROM_LOAD( "95004252.bin", 0x000000, 0x080000, BAD_DUMP CRC(2b5e88d5) SHA1(2ec98e569a185d0ce72bc09aedabbcaa1d4a7c9c) ) /* 'fixed bits' */ \
	ROM_LOAD( "95004253.bin", 0x080000, 0x080000, CRC(dcea501a) SHA1(c1fdcaa3f0a6143d26389d3f614af63cc48a2e58) ) \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* However the game we have wants the Grand Slam sounds? */ \
	ROM_LOAD( "95004169.bin", 0x000000, 0x080000, CRC(d2293b2f) SHA1(e3eed38a15885c5a6d63fc79d95f77c5225e9168) ) \
	ROM_LOAD( "95004170.bin", 0x080000, 0x080000, CRC(1da0db8f) SHA1(b3c37b5e02efe7267556d8603c362298ed55bb88) )
/* both 'On The Up' and 'Hyperactive' look for 'HYPE SOUNDS', do they both use this rom? */
#define sc_onup_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* HYPE SOUNDS */ \
	ROM_LOAD( "95008298.bin", 0x0000, 0x100000, CRC(6dcdcbfa) SHA1(1742f54ddd74d7513e8efb8cecfff574263c65df) )
#define sc_phgcl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR1623 CLASS PHAR GOLD SOUNDS11 */ \
	ROM_LOAD( "95008475.bin", 0x0000, 0x100000, CRC(30f77b7e) SHA1(bbb16020f33c8440344670beec8b13dd554be858) )
#define sc_clbmn_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR1955 C M MONEYBAGS SOUNDS11 */ \
	ROM_LOAD( "95008666.bin", 0x000000, 0x100000, CRC(6dcb4a96) SHA1(844dc67bacb7136a3c0f897a69ac5bcacaa4968a) ) \
	ROM_LOAD( "95008667.bin", 0x100000, 0x100000, CRC(5243784c) SHA1(771248de9fa3446ad6fe1bd35b240ad6ae23f008) )
#define sc_mopl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR3408 MPOLY PROP LAD SOUNDS11 */ \
	ROM_LOAD( "95009160.bin", 0x000000, 0x100000, CRC(797bf91a) SHA1(e467a07b512bf753e212665367fcca481e27da54) ) \
	ROM_LOAD( "95009161.bin", 0x100000, 0x100000, CRC(5ea75d9c) SHA1(eb6d96b8a5af879b3eff5b5d501ed81e46b1e1af) )
#define MAZOOMA_POWERBALL_SOUNDS \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* POWERBALL SOUNDS */ \
	ROM_LOAD( "95004218", 0x000000, 0x080000, CRC(4e90206c) SHA1(475fe57a4f08b17acaf63f0863a705fd70b910db) ) \
	ROM_LOAD( "95004219", 0x080000, 0x080000, CRC(f7e344a2) SHA1(7f2411373fdd49d7ef3c6990fb3cc1fe93ca3348) )


/* missing ROMs below */

#define sc_bkngx_others \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890139.bin", 0x0000, 0x5000, CRC(03d69c54) SHA1(9e1e79378874092a19eb31af8d8b5b2422364086) ) \
	ROM_LOAD( "95890140.bin", 0x0000, 0x5000, CRC(1f006ce8) SHA1(4ce346e7b72546d5d6d9137a5fa6c449c6da292d) ) \
	ROM_LOAD( "95890141.bin", 0x0000, 0x5000, CRC(dc5a4e78) SHA1(31aefb5dcf67eafdd9fc83e086fc00e678f0093a) ) \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_bob_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
		\
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890312.bin", 0x0000, 0x5000, CRC(9dfc2404) SHA1(c35f4197c0e0cd45b35f70cce166497de385b212) ) \
	ROM_LOAD( "95890313.bin", 0x0000, 0x5000, CRC(e86a6030) SHA1(c7fd1ae68aa60c448a31eb9d4103aa0a8c1892d0) ) \
	ROM_LOAD( "95890314.bin", 0x0000, 0x5000, CRC(76d0ac6c) SHA1(83d150737f942abf19a480b06dc09e1063bb0e64) )
#define sc_clue_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "clue sounds", 0x0000, 0x100000, NO_DUMP ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890356.bin", 0x0000, 0x5000, CRC(fd78e03f) SHA1(029e2fd7e9cff3793d905d192c889873ad2a0d26) ) \
	ROM_LOAD( "95890357.bin", 0x0000, 0x5000, CRC(88eea40b) SHA1(5a730add3f9941dfcbfa5cd8f6a6142d19065482) ) \
	ROM_LOAD( "95890358.bin", 0x0000, 0x5000, CRC(16546857) SHA1(d8b81b3df5e041c83c4f359c487165a6a2d5c513) )
#define sc_ducks_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890347.bin", 0x0000, 0x5000, CRC(9faa9c11) SHA1(5e2add80ad17a27dd7b5b0a298ea6753b6adca11) ) \
	ROM_LOAD( "95890348.bin", 0x0000, 0x5000, CRC(ea3cd825) SHA1(0ecef82b32b0166ef5f8deecd7a85fd52fc66bf8) ) \
	ROM_LOAD( "95890349.bin", 0x0000, 0x5000, CRC(74861479) SHA1(7ba29931336985b96995688afdfe2b83a7602a84) )
#define sc_pp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "pink sounds", 0x0000, 0x100000, NO_DUMP ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890380.bin", 0x0000, 0x5000, CRC(1ba73039) SHA1(6f07fa6f412e92c5a55be15f8cce7aa6f5e8dca0) ) \
	ROM_LOAD( "95890381.bin", 0x0000, 0x5000, CRC(0771c085) SHA1(5eb89b14c03cac0f56d057dc2c0c7210cc9443ea) ) \
	ROM_LOAD( "95890382.bin", 0x0000, 0x5000, CRC(c42be215) SHA1(4da4820ee08a102a2a49ce64a49f66c9548b8ab1) ) \
	ROM_LOAD( "95890398.bin", 0x0000, 0x5000, CRC(007c15b3) SHA1(8330f7e8d2dc062118016d8f003df48930a5aeb8) ) \
	ROM_LOAD( "95890399.bin", 0x0000, 0x5000, CRC(1caae50f) SHA1(1e978f7f4851384cd800ebecd9beeff1a5df8156) ) \
	ROM_LOAD( "95890400.bin", 0x0000, 0x5000, CRC(dff0c79f) SHA1(aae28c245a672417962cd19388e4d5c401708911) )

#define sc_gcb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_hdd_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_hyde_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_jiggr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_jive_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_jbuck_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_jjc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_kkong_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_knok_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_mbags_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_m2m_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_maxcc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_monod_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_mwwcl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_motor_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_outlw_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_pwrbq_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_pwcrz_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_ibiza_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )

#define sc_quart_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc5tbox_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_rmo_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define PR1918_POTS_OF_GOLD_SOUNDS11 \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "pr1918 pots of gold sounds11", 0x000000, 0x100000, NO_DUMP )
#define sc_rtclb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_sf_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_vmnv_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008095_viva_mexico_pr6984", 0x000000, 0x100000, NO_DUMP )
#define sc_abra_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "abra.snd", 0x0000, 0x100000, NO_DUMP )
#define sc_broll_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "rol sounds", 0x0000, 0x100000, NO_DUMP )
#define sc_cashm_german_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "cash german sounds", 0x000000, 0x100000, NO_DUMP )
#define sc_frboo_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_supst_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_ufi_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_wadzl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_wag_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_wldbn_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_wthng_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_wthnm_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_wtc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc4s16_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_bgold_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_cconx_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "mtom sounds", 0x000000, 0x100000, NO_DUMP )
#define sc_ccrus_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "cash sounds", 0x000000, 0x100000, NO_DUMP )
#define sc_cerup_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "cash erruption sounds", 0x000000, 0x100000, NO_DUMP )
#define sc_cexpl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "cash explosion sounds", 0x000000, 0x100000, NO_DUMP )
#define sc_chand_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "pr7108 cash in hand sounds11", 0x000000, 0x100000, NO_DUMP )
#define sc_crzwl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "crazy world sounds", 0x0000, 0x100000, NO_DUMP )
#define sc_cinv_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "pr6809 cashinvaders sounds", 0x0000, 0x100000, NO_DUMP )
#define sc_crsgc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_crcpt_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_dracp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_dndpc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_fdice_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_gx3_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_glad_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_hotdg_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_monl_sounds \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "monl sounds", 0x0000, 0x100000, NO_DUMP )
#define sc_nunsb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_pwrpl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_rssh_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_r2r_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_rollo_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_r66_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_sfts_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_srace_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_s2k_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_splgb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_spred_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_swywm_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_sumit_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "sumit sounds", 0x0000, 0x100000, NO_DUMP )
#define sc_tsmp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_tpsht_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc5pompa_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_viper_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* 95008474.bin = 95008910.bin          sc4hiss    Hissing Quid (Qps) (Scorpion 4) */
#define sc_srrcl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_mrrcl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_ggcas_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_gnc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_adga_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_adgtc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_adren_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_bigdl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "bigdealsnd", 0x0000, 0x100000, NO_DUMP )
#define sc_bingb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_blue_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "brinse sounds", 0x0000, 0x100000, NO_DUMP )
#define sc_cbrun_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_brix_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "brix sounds", 0x0000, 0x100000, NO_DUMP )
#define sc_bugs_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "pr7109 bugs money sounds11", 0x0000, 0x100000, NO_DUMP )
#define sc_btiab_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_btrip_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define COROST_TRIPLE_SOUNDS \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "corost_triple_sounds", 0x0000, 0x100000, NO_DUMP )

#define sc_clucl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "clucl sounds", 0x000000, 0x100000, NO_DUMP )

#define sc_mombc_others \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890597.bin", 0x0000, 0x5000, CRC(0d24f414) SHA1(85912d9e07995ffd6682d1ef65e4d71090b6584f) ) \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )

#define sc_revo_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "rev sounds", 0x0000, 0x100000, NO_DUMP )
#define sc_wild_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	\
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890339.bin", 0x0000, 0x5000, CRC(e3fbc85c) SHA1(8fc235f53b9359d378f86819e04377e7c102428c) ) \
	ROM_LOAD( "95890340.bin", 0x0000, 0x5000, CRC(ff2d38e0) SHA1(3b62d6c6448312f999502c244d483df8a60be77a) ) \
	ROM_LOAD( "95890341.bin", 0x0000, 0x5000, CRC(3c771a70) SHA1(bf47ecdea05757d9636526308f4aeabe3748d307) )

#define sc_fbcrz_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_sdr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
#define sc_wdw_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )
/* only the parent set wants 'WILD SOUNDS' but not the same as Wet N Wild */
#define sc_nudit_wild_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "wild sounds", 0x000000, 0x080000, NO_DUMP )

/* confirmed bad / incomplete dumps */

#define sc_slih_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* missing first half of ROM, has sample table at the end with 'SLIH SOUNDS' */ \
	ROM_LOAD( "s_l_i_ho.snd", 0x0000, 0x080000, BAD_DUMP CRC(13abf434) SHA1(99d7a323c03448827d16bcfb4358c09ab691b500) )
// missing first half of both roms
#define sc_stl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR6905 SKYS SOUNDS11 */ \
	ROM_LOAD( "sk_t_lim.sn1", 0x000000, 0x080000, BAD_DUMP CRC(1b88bf7d) SHA1(76a8453fd2f8375030f69a3a569b45daceb3e4f7) ) \
	ROM_LOAD( "sk_t_lim.sn2", 0x080000, 0x080000, BAD_DUMP CRC(b3ecf658) SHA1(bf4ce85a3662df6dacfa42e3b4f9dfd6bba43b24) )
#define sc_stag_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR6915 STAG SOUNDS11 */ \
	ROM_LOAD( "95008017.lo", 0x000000, 0x100000, BAD_DUMP CRC(dab74220) SHA1(321f765e1908b65ec9d3e64bb33533d78c9cc58a) ) /* start of rom is blank?! */ \
	ROM_LOAD( "95008018.hi", 0x100000, 0x100000, CRC(ba328f08) SHA1(99880f22c2cac61b32c3e2a574b9b30daefefb3e) )
#define sc_tfclb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR6938 TUTTI FRUTTI SOUNDS11 */ \
	ROM_LOAD( "95008043.bin", 0x000000, 0x100000, BAD_DUMP CRC(b168cec4) SHA1(42b891ba918c7063420688c99dc72609801a1987) )  /* start of rom is blank?! */ \
	ROM_LOAD( "95008044.bin", 0x100000, 0x100000, CRC(6074fa77) SHA1(1d4fa88a7ba2b020042ef3681e0f8eb732705d55) )
#define sc_crzcs_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR6906 CRCS SOUNDS11 */ \
	ROM_LOAD( "95008005.lo", 0x00000, 0x100000, BAD_DUMP CRC(0b10db9f) SHA1(e0ea7b4f2dd98ef23ab42ffe7a7892e5327f646e) ) /* start of rom is blank?! */ \
	ROM_LOAD( "95008006.hi", 0x100000, 0x100000, CRC(be2cdfd5) SHA1(87e25d6e26052dca8d79fbda705b2df1c06e2c67) )

// I don't think there is a good / complete set of sound roms in here, all the sets we have reject any configuration of this
// and the dumps are a mess - we the header says sound table is at 0x001E33FB which would put it in the final (4th) 512kb block,
// however we only have 3 valid blocks of sound data
#define sc_clbtm_others \
	ROM_REGION( 0x1000000, "others2", ROMREGION_ERASE00 ) \
	/* first 512kb of this = first 512kb of 95008055.bin, rest is garbage? */ \
	ROM_LOAD( "95008055.p1", 0x00000, 0x80000, BAD_DUMP CRC(ef474fd3) SHA1(e7427184683603b57a3a8b37452fa6ec7a41e34c) ) \
	ROM_IGNORE(0x780000) \
	\
	ROM_REGION( 0x1000000, "ymz", ROMREGION_ERASE00 ) \
	/* both halves of this valid sound data? */ \
	ROM_LOAD( "95008055.bin", 0x0000, 0x100000, CRC(df9ae6e3) SHA1(5766cb1749aa92c34a76270a641f7a9302cc44d7) ) \
	/* first 512kb of this = unique sound data? rest is garbage? */ \
	ROM_LOAD( "95008056.p2", 0x100000, 0x800000, BAD_DUMP CRC(39b1b566) SHA1(937ec27964124b92b75d4b37d09a35585baa68c6) )

// header lists sound table as being at 0x001FEDBF indicating this should be 2 * 0x100000 roms?
#define sc_alad_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* CAVE SOUNDS */ \
	ROM_LOAD( "95008075.bin", 0x0000, 0x080000, BAD_DUMP CRC(c9306583) SHA1(8bccb9529e7d24be7b4f3ffda0d35780a170be43) )
/* header says sound table is at 0x0FF65F so we're either missing a ROM or this is half size */
#define sc_ufg_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004167.bin", 0x0000, 0x080000, BAD_DUMP CRC(9dcd31cc) SHA1(2ce9e167debbc2315b4be964116fdc35e19b6046) )
/* header says sound table is at 0x0FE876 so we're either missing a ROM or this is half size */
#define sc_ttp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004148.bin", 0x0000, 0x080000, BAD_DUMP CRC(6b352d5b) SHA1(b32f08828aad1c8cf0281faa387c9a1536fbd382) )
/* header says sound table is at 0x0FE7A6 so we're either missing a ROM or this is half size */
#define sc_lions_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004142.bin", 0x0000, 0x080000, BAD_DUMP CRC(aee399f7) SHA1(54c9683f2f3159122ef35855fe19380f1a2771ca) )
/* header says sound table is at 0x1F7D58 so we're either missing a ROM or this is half size */
#define sc_cyccl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* CLCC SOUNDS */ \
	ROM_LOAD( "95008310.bin", 0x0000, 0x100000, BAD_DUMP CRC(4dddf82d) SHA1(4bab8cdc5f6bf59c3686dfd01f2f96858f776df3) )
/* header says sound table is at 0x1E8460 so we're either missing a ROM or this is half size */
#define sc_cyc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) /* PR2058 COUNT Y CASH SOUNDS11 */ \
	ROM_LOAD( "95008249.bin", 0x0000, 0x100000, BAD_DUMP CRC(e8eac90a) SHA1(f79dc6912a8fbeb12c577c402d84ef4780bf4333) )

/* wrong hardware below */

#define QPS_GOLDENPALCE_SOUNDS \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* Golden Palace (QPS) No Header? looks like a UPD rom instead */ \
	ROM_LOAD( "61000120.bin", 0x0000, 0x080000, CRC(a2cdc045) SHA1(5f2534683f2399e2fe0e8686be0f2e42e1b04a74) )
#define sc_casry_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* not for either of these games? */ \
	ROM_LOAD( "casroysnd.bin", 0x00000, 0x80000, CRC(cf1d4b59) SHA1(1b2bc74c6fcc43197a6f295bc34554da01f7b517) )

#endif // MAME_INCLUDES_BFP_SC4_H
