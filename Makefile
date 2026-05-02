.PHONY: cpp-build cpp-test java-test py-test up down smoke zip

cpp-build:
	cmake -S cpp -B build/cpp -DCMAKE_BUILD_TYPE=Release
	cmake --build build/cpp -j

cpp-test: cpp-build
	./build/cpp/protocol_tests

java-test:
	cd java-mission-server && ./mvnw test

py-test:
	cd python-tools && python -m pip install -r requirements.txt && pytest -q

up:
	docker compose up --build

down:
	docker compose down -v

smoke:
	bash scripts/smoke-test.sh

zip:
	cd .. && zip -r sat-gcs-production.zip sat-gcs-production -x '*/target/*' '*/node_modules/*' '*/build/*'
