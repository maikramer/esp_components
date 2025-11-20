# CMakeLists.txt principal do componente esp_components
# Este componente agrupa todos os subcomponentes
# Os subcomponentes são detectados automaticamente pelo ESP-IDF em subdiretórios
# Este componente raiz é necessário quando o componente é adicionado via idf_component.yml

# Componente wrapper vazio - os subcomponentes são detectados automaticamente
idf_component_register()
