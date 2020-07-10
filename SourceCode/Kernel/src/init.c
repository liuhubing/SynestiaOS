#include <interrupt.h>
#include <stdlib.h>
#include <synestia_os_hal.h>
#include <kheap.h>
#include <vmm.h>
#include <sched.h>
#include <gpu.h>
#include <gui_button.h>
#include <gui_window.h>
#include <gui_label.h>
#include <gfx2d.h>
#include <gui_panel.h>
#include <gui_container.h>
#include <font8bits.h>

void print_splash() {
    const char *str = "   _____                       _   _       \n"
                      "  / ____|                     | | (_)      \n"
                      " | (___  _   _ _ __   ___  ___| |_ _  __ _ \n"
                      "  \\___ \\| | | | '_ \\ / _ \\/ __| __| |/ _` |\n"
                      "  ____) | |_| | | | |  __/\\__ \\ |_| | (_| |\n"
                      " |_____/ \\__, |_| |_|\\___||___/\\__|_|\\__,_|\n"
                      "          __/ |                            \n"
                      "         |___/                          \n";
    print(str);
}

uint32_t ti = 0;

void demo_desktop() {
    gfx2d_draw_bitmap(0, 0, 1024, 768, desktop());
    printf("[Desktop]: render\n");

    uint32_t *barBuffer = (uint32_t *) kheap_alloc(1024 * 32 * 4);

    for (int i = 0; i < 1024; i++) {
        for (int j = 0; j < 31; j++) {
            barBuffer[j * 1024 + i] = ((FLUENT_PRIMARY_COLOR >> 16) & 0xFF - j) << 16 | ((FLUENT_PRIMARY_COLOR >> 8) & 0xFF - j) << 8 | (FLUENT_PRIMARY_COLOR & 0xFF - j);
        }
    }
    for (int i = 0; i < 1024; i++) {
        barBuffer[31 * 1024 + i] = 0x999999;
    }
    gfx2d_draw_bitmap(0, 0, 1024, 32, barBuffer);
    gfx2d_draw_logo(0, 0, 0x000000);

    GUIButton synestiaOSButton;
    gui_button_create(&synestiaOSButton);
    synestiaOSButton.component.size.height = 32;
    synestiaOSButton.component.padding.top = 12;
    synestiaOSButton.component.colorMode = TRANSPARENT;
    synestiaOSButton.component.foreground.r = 0x00;
    synestiaOSButton.component.foreground.g = 0x00;
    synestiaOSButton.component.foreground.b = 0x00;
    char int32s[10] = {'\0'};
    char *intStr = itoa(ti, &int32s, 10);
    gui_button_init(&synestiaOSButton, 32, 0, intStr);
    gui_button_draw(&synestiaOSButton);

    GUILabel synestiaOSLabel2;
    gui_label_create(&synestiaOSLabel2);
    synestiaOSLabel2.component.colorMode = TRANSPARENT;
    gui_label_init(&synestiaOSLabel2, 300, 4, "Welcome to Synestia Operation System.");
    gui_label_draw(&synestiaOSLabel2);

    GUIWindow window;
    gui_window_create(&window);
    gui_window_init(&window, 100, 100, "SynestiaOS");
    gui_window_draw(&window);

    GUIWindow window1;
    gui_window_create(&window1);
    gui_window_init(&window1, 150, 150, "SynestiaOS 1");
    gui_window_draw(&window1);

    GUIButton ok;
    gui_button_create(&ok);
    synestiaOSButton.component.size.height = 32;
    synestiaOSButton.component.padding.top = 12;
    gui_button_init(&ok, 0, 0, "Inner Window Button");

    GUILabel label;
    gui_label_create(&label);
    gui_label_init(&label, 0, 42, "Inner Window Label");

    GUILabel label3;
    gui_label_create(&label3);
    gui_label_init(&label3, 0, 0, "Inner Label 1");

    GUILabel label4;
    gui_label_create(&label4);
    gui_label_init(&label4, 0, 0, "Inner Label 2");

    GUIPanel panel2;
    gui_panel_create(&panel2);
    panel2.component.size.width = 150;
    panel2.component.size.height = 50;
    panel2.component.background.r = 0x00;
    panel2.component.background.g = 0x00;
    gui_panel_init(&panel2, 10, 100);
    gui_panel_add_children(&panel2, &(label4.component));

    GUIPanel panel;
    gui_panel_create(&panel);
    panel.component.background.r = 0x00;
    gui_panel_init(&panel, 0, 200);
    gui_panel_add_children(&panel, &(label3.component));
    gui_panel_add_children(&panel, &(panel2.component));

    GUILabel label4container;
    gui_label_create(&label4container);
    gui_label_init(&label4container, 0, 0, "Label for container");

    GUIButton button4container;
    gui_button_create(&button4container);
    button4container.component.size.height = 32;
    button4container.component.padding.top = 12;
    gui_button_init(&button4container, 0, 0, "Button for container");

    GUIContainer container;
    gui_container_create(&container);
    container.component.background.b = 0x00;
    gui_container_init(&container, 240, 0, VERTICAL);
    gui_container_add_children(&container, (&label4container.component));
    gui_container_add_children(&container, (&button4container.component));

    GUIWindow window2;
    gui_window_create(&window2);
//    gui_window_add_children(&window2, &(ok.component));
//    gui_window_add_children(&window2, &(label.component));
//    gui_window_add_children(&window2, &(panel.component));
//    gui_window_add_children(&window2, &(container.component));
    gui_window_init(&window2, 200, 200, "SynestiaOS 2");
    gui_window_draw(&window2);
}

void xx() {
    ti++;
    demo_desktop();
}

TimerHandler t;

void kernel_main(void) {

    print_splash();

    vmm_init();

    kheap_init();

    init_bsp();

    init_interrupt();

    schd_init();

    schd_switch_to(thread_create_idle_thread(0));

    gpu_init();

    t.node.next = nullptr;
    t.node.prev = nullptr;
    t.timer_interrupt_handler = &xx;

    register_time_interrupt(&t);

    demo_desktop();
}
