	section .text
	global VAGaddr
	global VAG_data_size
	
	section .text
VAGname	equs	"trak.vag"
	
VAGaddr:
	incbin \VAGname\
