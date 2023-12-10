pipeline {
    agent {
        dockerfile {
            filename 'Dockerfile'
            dir '.'
            additionalBuildArgs '--target esp_base'
        }
    }
    stages {
        stage("Keep build") {
            agent any
            when {
                branch 'master'
            }
            steps {
                script {
                    currentBuild.setKeepLog(true)
                }
            }
        }
        stage('Build') {
            steps {
                sh(label: 'Building', script: """#!/bin/bash
                    . ~/esp/esp-idf/export.sh && cd /var/jenkins/workspace/Github_ESPStreamer_${env.GIT_BRANCH} && idf.py build -DBUILD_NUM=${env.BUILD_NUMBER}
                """)    
            }  
            post {
                success {
                    dir('build') {
                        sh(label: 'Combining binary files', script: """#!/bin/bash
                            . ~/esp/esp-idf/export.sh && esptool.py --chip ESP32 merge_bin -o merged_espstreamer.bin --flash_mode dio --flash_size 2MB 0x1000 bootloader/bootloader.bin 0x8000 partition_table/partition-table.bin 0x10000 ESPStreamer.bin
                            mkdir espstreamer
                            echo "${env.BUILD_NUMBER}" > esp_firmware_version && mv esp_firmware_version espstreamer
                            pigz -k -z -9 bootloader/bootloader.bin
                            pigz -k -z -9 ESPStreamer.bin && mv ESPStreamer.bin.zz espstreamer.bin.zz
                            mv espstreamer.bin.zz espstreamer && mv partition_table/partition-table.bin espstreamer && mv bootloader/bootloader.bin.zz espstreamer    
                        """)
                        archiveArtifacts artifacts: 'espstreamer/**/*'
                        archiveArtifacts artifacts: 'merged_espstreamer.bin'
                    }
                }
            }           
        }
    }
} 