# CMakeLists.txt principal do componente esp_components
# Este arquivo registra o componente principal que contém todos os subcomponentes

# O ESP-IDF automaticamente detecta e registra subcomponentes em subdiretórios
# que contenham CMakeLists.txt. Portanto, não precisamos registrar manualmente
# cada subcomponente aqui.

# Este componente principal não tem arquivos fonte próprios, apenas agrupa
# os subcomponentes. Se necessário adicionar código comum, adicione aqui.

idf_component_register()

