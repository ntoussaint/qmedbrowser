
add_library(QMedBrowsing
  QMetaDataWidget.cxx
  QMedImageItem.cxx
  QMedImageListWidget.cxx
  QMedBrowserWindow.cxx
  QMedBrowserSettingsDialog.cxx
  QMedImageReaderThread.cxx
  )

target_link_libraries(QMedBrowsing
  Qt5::Core
  Qt5::Widgets
  Qt5::Concurrent
  ${ITK_LIBRARIES}
  )

install(TARGETS QMedBrowsing
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT libraries)
