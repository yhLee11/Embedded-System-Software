#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xad1a7def, "module_layout" },
	{ 0xfe990052, "gpio_free" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x1d11aa3a, "cdev_del" },
	{ 0xc1514a3b, "free_irq" },
	{ 0xd6b8e852, "request_threaded_irq" },
	{ 0x72c365ef, "gpiod_to_irq" },
	{ 0x403f9529, "gpio_request_one" },
	{ 0xc6f46339, "init_timer_key" },
	{ 0x88647e86, "cdev_add" },
	{ 0x7a3acc89, "cdev_init" },
	{ 0xb4673642, "cdev_alloc" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xc38c83b8, "mod_timer" },
	{ 0x526c3a6c, "jiffies" },
	{ 0xd697e69a, "trace_hardirqs_on" },
	{ 0xffb0dcd9, "gpiod_set_raw_value" },
	{ 0x350a004, "gpio_to_desc" },
	{ 0x24d273d1, "add_timer" },
	{ 0xec3d2e1b, "trace_hardirqs_off" },
	{ 0xfcec0987, "enable_irq" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x3ce4ca6f, "disable_irq" },
	{ 0x7c32d0f0, "printk" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "C33A381A797874F90DCF334");
