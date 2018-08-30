pipeline {
    parameters {
        string(name: 'PROBE_TYPE', description: 'probe to build; could be sysdig, sysdigcloud-probe, falco', defaultValue: 'sysdig-probe')
        string(name: 'TARGET_TAG', description: 'tag of git repo', defaultValue: '0.23.1')
    }

    agent { label 'probe-builder-test' }
    stages {
        stage ('preparation') {
            steps {
                sh 'hostname'
                sh 'uname -a'
                sh 'gcc --version -v'
                sh 'g++ --version -v'
                sh 'pwd -P'
                sh 'df -h'
                dir('sysdig') { checkout scm }
                sh 'pwd -P; df -h'
                sh 'ls -l'
                sh 'echo build dokcer images of various builders ...'
                sh 'sysdig/scripts/build-builder-containers.sh'
                sh 'docker images'
                sh 'docker ps -a'
                sh 'docker ps -aq -f "name=fedora-atomic-build|amazon-linux-build|ol6-buildol7-build" | xargs --no-run-if-empty docker rm -f'
                sh 'docker ps -a'
                sh 'mkdir -p probe/output'
                sh 'echo ready for probe build: ${PROBE_TYPE}, code tag ${TARGET_TAG}'
            }
        }

        stage ('compilation') {
            steps {
                parallel (
                    'info':   { sh 'echo "git repo branch: ${BRANCH_NAME}" && pwd -P && df -h' },
                    'ubuntu': { 
                        sshagent(['4399087a-3e99-41e5-9dbe-a70a554672c8']) {
                            sh 'mkdir -p probe/ubuntu        && cd probe/ubuntu        && bash -x ../../sysdig/scripts/build-probe-binaries ${PROBE_TYPE} ${TARGET_TAG} stable Ubuntu && cp -u output/*${TARGET_TAG}* ../output/ && echo ubuntu finished' 
                        }
                    },
                     "rhel" : { 
                        sshagent(['4399087a-3e99-41e5-9dbe-a70a554672c8']) {
                             sh 'mkdir -p probe/rhel          && cd probe/rhel          && bash -x ../../sysdig/scripts/build-probe-binaries ${PROBE_TYPE} ${TARGET_TAG} stable RHEL && cp -u output/*${TARGET_TAG}* ../output/ && echo rhel finished' 
                        }
                    }
                )
            }
        }
    
        stage ('s3 publishing') {
	    when { branch "${BRANCH_NAME}" }
            steps {
                sh 'hostname'
                sh 'uname -a'
                sh 'pwd -P'
                sh 'echo workspace = $WORKSPACE'
                sh 'df -h'
                sh 'ls -l probe/output/'
                sh 'echo "number of total files = $(ls -l probe/output/ | wc -l)" '
            }
        }
    }
}
