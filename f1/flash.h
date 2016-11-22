#ifndef __FLASH_H
#define __FLASH_H

template<
	typename DATA_ITEM,
	const uint32_t START_ADDR,
	const int SECTOR_COUNT>
struct FLASH_T {
	static constexpr FLASH_TypeDef *flash = FLASH;
	static constexpr uint16_t *sectors = (uint16_t *) START_ADDR;
	static constexpr uint16_t *temp_sector = (uint16_t *) (START_ADDR + SECTOR_COUNT * 1024);
	static constexpr DATA_ITEM *items = (DATA_ITEM *) START_ADDR;
	static constexpr int max_sectors = SECTOR_COUNT;
	static constexpr int items_per_sector = 1024 / sizeof(DATA_ITEM);
	static constexpr int max_items = items_per_sector * max_sectors;

	static void init(void) {
		RCC->AHBENR |= RCC_AHBENR_FLITFEN;
		static_assert(items_per_sector * sizeof(DATA_ITEM) == 1024, "Data must fit into sector");
	}

	static void disable(void) {
		RCC->AHBENR &= ~RCC_AHBENR_FLITFEN;
	}

	static void unlock(void) {
		flash->KEYR = 0x45670123;
		flash->KEYR = 0xcdef89ab;
	}

	static void lock(void) {
		flash->CR |= FLASH_CR_LOCK;
	}

	static int find_last_item(void) {
		DATA_ITEM *last = items;
		int current_item = 0;

		while (current_item < max_items &&
		       (items[current_item].valid() ||
		       items[current_item] < items[(current_item + 1) % max_items])) {
			current_item++;
		}
		if (current_item == max_items) {
			current_item = 0;
		}
		return current_item;
	}

	static void write(void *dst, void* src, int bytes) {
		uint16_t *s = reinterpret_cast<uint16_t*>(src);
		uint16_t *d = reinterpret_cast<uint16_t*>(dst);

		unlock();
		flash->CR &= ~(FLASH_CR_PER | FLASH_CR_MER);
		flash->CR |= FLASH_CR_PG;
		for (int i = 0; i < bytes / 2; i++) {
			*d++ = *s++;
			while (flash->SR & FLASH_SR_BSY);
			while (!(flash->SR & FLASH_SR_EOP));
			flash->SR &= ~FLASH_SR_EOP;
		}
		lock();

	}

	static void write(DATA_ITEM *data, int item) {
		if (items[item].valid()) {
			int item_offset = item * sizeof(DATA_ITEM) % 1024;
			int item_sector = item * sizeof(DATA_ITEM) / 1024;
			if (item_offset != 0) {
				erase(temp_sector);
				write(temp_sector, &items[item_sector * sizeof(DATA_ITEM)], item_offset);
				erase(&items[item]);
				write(&items[item_sector * sizeof(DATA_ITEM)], temp_sector, item_offset);
			} else {
				erase(&items[item]);
			}
		}
		write(&items[item], data, sizeof(DATA_ITEM));
	}

	static void erase(void *address) {
		unlock();
		flash->CR &= ~(FLASH_CR_MER | FLASH_CR_PG);
		flash->CR |= FLASH_CR_PER;
		flash->AR = reinterpret_cast<uint32_t>(address);
		flash->CR |= FLASH_CR_STRT;
		while (flash->SR & FLASH_SR_BSY);
		while (!(flash->SR & FLASH_SR_EOP));
		flash->SR &= ~FLASH_SR_EOP;
		lock();
	}
};

#endif
