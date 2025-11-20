# CMakeLists.txt principal do componente esp_components
# Este componente agrupa todos os subcomponentes
# Os subcomponentes são detectados automaticamente pelo ESP-IDF em subdiretórios
# Este componente raiz é necessário quando o componente é adicionado via idf_component.yml

idf_component_register()

